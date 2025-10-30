#include <iostream>
#include <string>
#include <curl/curl.h>
using namespace std;

CONST int NAME_ITERATOR_OFFSET = 6;
CONST int DESCRIPTION_ITERATOR_OFFSET = 13;
CONST int TEMPERATURE_ITERATOR_OFFSET = 6;
CONST int TEMPERATURE_FEELS_LIKE_ITERATOR_OFFSET = 12;
CONST string API_KEY = "e806bfd18411d235d782aca8b3ad1035";


// =============================
// Scrie răspunsul primit de la server într-un string
// =============================
size_t WriteCallback(void* contents, size_t size, size_t nmemb, string* userp) {
    size_t totalSize = size * nmemb;
    userp->append(static_cast<char*>(contents), totalSize);
    return totalSize;
}

// =============================
// Verifică dacă orașul introdus există (dacă API-ul a returnat "city not found")
// =============================
bool CheckIfCityExits(string& s) {
    int pos = s.find("city not found");
    if (pos != string::npos) {
        return false;
    }
    return true;
}

// =============================
// Extrage și afișează numele orașului din răspunsul JSON
// =============================
void GetCityData(string jsonResponse, int offset)
{
    int name_iterator = jsonResponse.find("name") + offset;
    cout << "City: ";
    while (jsonResponse[name_iterator] != ',') {
        if (jsonResponse[name_iterator] == '"')
            jsonResponse[name_iterator] = '\0';
        cout << jsonResponse[name_iterator];
        name_iterator++;
    }
    cout << endl;
}

// =============================
// Extrage și afișează temperatura actuală din JSON
// =============================
void GetTemperatureData(string jsonResponse, int offset)
{
    int temperature_iterator = jsonResponse.find("temp") + offset;
    cout << "Actual temperature: ";
    while (jsonResponse[temperature_iterator] != ',') {
        cout << jsonResponse[temperature_iterator];
        temperature_iterator++;
    }
    cout << " degrees Celsius";
    cout << endl;
}

// =============================
// Extrage și afișează temperatura resimțită ("feels_like")
// =============================
void FeelsLikeTemperatureData(string jsonResponse, int offset)
{
    int feels_like_temperature_iterator = jsonResponse.find("feels_like") + offset;
    cout << "Feels like temperature: ";
    while (jsonResponse[feels_like_temperature_iterator] != ',') {
        cout << jsonResponse[feels_like_temperature_iterator];
        feels_like_temperature_iterator++;
    }
    cout << " degrees Celsius";
    cout << endl;
}

// =============================
// Extrage și afișează descrierea condițiilor meteo ("description")
// =============================
void GetDescriptionData(string jsonResponse, int offset)
{
    int description_iterator = jsonResponse.find("description") + offset;
    cout << "Description: ";
    while (jsonResponse[description_iterator] != ',') {
        if (jsonResponse[description_iterator] == '"')
            jsonResponse[description_iterator] = '\0';

        cout << jsonResponse[description_iterator];
        description_iterator++;
    }
    cout << endl;
}

// =============================
// Trimite cererea către API și afișează datele meteo pentru orașul cerut
// =============================
void GetWeatherData(string& city_name, const string& api_key){
    CURL* curl;
    CURLcode res;
    string response;

    // Initializare libcurl
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if (curl) {
        // Set the API URL with parameters (replace with your API key and desired location)
        string url = "https://api.openweathermap.org/data/2.5/weather?q=" + city_name + "&appid=" + api_key + "&units=metric";

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

        // Set the write callback to capture the response
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        // Perform the request
        res = curl_easy_perform(curl);
        // Check for errors
        if (res != CURLE_OK) {
            cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << endl;
        }
        else {
            if (CheckIfCityExits(response)) {
                GetCityData(response, NAME_ITERATOR_OFFSET);
                GetTemperatureData(response, TEMPERATURE_ITERATOR_OFFSET);
                FeelsLikeTemperatureData(response, TEMPERATURE_FEELS_LIKE_ITERATOR_OFFSET);
                GetDescriptionData(response, DESCRIPTION_ITERATOR_OFFSET);
            }
            else
                cout << "City does not exists";
            
        }
        // Clean up
        curl_easy_cleanup(curl);
    }
}

// =============================
// Înlocuiește spațiile din numele orașului cu "%20" (pentru URL valid)
// =============================
void SpaceAsURLEncoded(string& s) {
    int pos = s.find(' ');
    if (pos != string::npos) {
        s.replace(pos, 1, "%20");
    }
}

// =============================
// Curăță consola (cls pentru Windows, clear pentru Linux/macOS)
// =============================
void ClearConsole() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif 
}

int main() {
    string city_name;
    int option;

    while (true) {
        ClearConsole();
        cout << "======================" << endl;
        cout << "     Weather APP      " << endl;
        cout << "======================" << endl;
        cout << "1. Check weather" << endl;
        cout << "2. Exit" << endl;
        cout << "Choose an option: ";
        cin >> option;

        // Fix cin issue if invalid input
        if (cin.fail()) {
            cin.clear();
            cout << "Invalid input! Try again.\n";
            continue;
        }

        cin.ignore(); // Clear newline from buffer

        if (option == 1) {
            cout << "Please enter a city: ";
            getline(cin, city_name);
            SpaceAsURLEncoded(city_name);
            GetWeatherData(city_name, API_KEY);
            cout << endl;
            cout << "Press ENTER to return to menu...";
            cin.get();
        }
        else if (option == 2) {
            cout << "Exiting..." << endl;
            break;
        }
        else {
            cout << "Invalid option, please try again!" << endl;
        }
    }

    curl_global_cleanup();
    return 0;
}