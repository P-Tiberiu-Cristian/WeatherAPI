#include <iostream>
#include <string>
#include <curl/curl.h>     // Biblioteca cURL — folosita pentru a trimite cereri HTTPS catre OpenWeatherMap API
using namespace std;

// =============================
// CONSTANTE GLOBALE
// =============================

// Offset-uri pentru pozitiile unde incep anumite campuri in raspunsul JSON primit de la API
CONST int NAME_AND_TEMPERATURE_ITERATOR_OFFSET = 6;
CONST int DESCRIPTION_ITERATOR_OFFSET = 13;
CONST int TEMPERATURE_FEELS_LIKE_ITERATOR_OFFSET = 12;

// Cheia API (folosita pentru autentificare in OpenWeather API)
CONST string API_KEY = "e806bfd18411d235d782aca8b3ad1035";


// =============================
// FUNCTIE: WriteCallback
// =============================
// Aceasta functie este apelata de cURL atunci cand primeste date de la server
// Ea adauga continutul primit (raspunsul JSON) intr-un string
size_t WriteCallback(void* contents, size_t size, size_t nmemb, string* userp) {
    size_t totalSize = size * nmemb;                    
    userp->append(static_cast<char*>(contents), totalSize);  
    return totalSize;                                  
}


// =============================
// FUNCTIE: CheckIfCityExits
// =============================
// Verifica daca orasul introdus exista, cautand textul "city not found" in raspunsul JSON
// Daca apare acest text, API-ul nu a gasit orasul respectiv
bool CheckIfCityExits(string& s) {
    int pos = s.find("city not found");
    if (pos != string::npos) {
        return false; 
    }
    return true;     
}


// =============================
// FUNCTIE: GetCityData
// =============================
// Extrage si afiseaza numele orasului din raspunsul JSON primit de la API
void GetCityData(string jsonResponse, int offset)
{
    int name_iterator = jsonResponse.find("name") + offset; // Gaseste pozitia "name"
    cout << "City: ";

    // Parcurge caracterele de dupa "name" pana la aparitia virgulei
    while (jsonResponse[name_iterator] != ',') {
        if (jsonResponse[name_iterator] == '"')
            jsonResponse[name_iterator] = '\0'; 
        cout << jsonResponse[name_iterator];
        name_iterator++;
    }
    cout << endl;
}


// =============================
// FUNCTIE: GetTemperatureData
// =============================
// Extrage si afiseaza temperatura actuala din JSON (campul "temp")
void GetTemperatureData(string jsonResponse, int offset)
{
    int temperature_iterator = jsonResponse.find("temp") + offset;
    cout << "Actual temperature: ";

    // Afiseaza caracterele pana la virgula 
    while (jsonResponse[temperature_iterator] != ',') {
        cout << jsonResponse[temperature_iterator];
        temperature_iterator++;
    }

    cout << " degrees Celsius" << endl;
}


// =============================
// FUNCTIE: FeelsLikeTemperatureData
// =============================
// Extrage si afiseaza temperatura resimtita ("feels_like") din JSON
void FeelsLikeTemperatureData(string jsonResponse, int offset)
{
    int feels_like_temperature_iterator = jsonResponse.find("feels_like") + offset;
    cout << "Feels like temperature: ";

    while (jsonResponse[feels_like_temperature_iterator] != ',') {
        cout << jsonResponse[feels_like_temperature_iterator];
        feels_like_temperature_iterator++;
    }

    cout << " degrees Celsius" << endl;
}


// =============================
// FUNCTIE: GetDescriptionData
// =============================
// Extrage si afiseaza descrierea conditiilor meteo ("description")
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
// FUNCTIE: GetWeatherData
// =============================
// Trimite cererea catre API, primeste raspunsul si afiseaza toate datele meteo
// Aici se folosesc functiile cURL pentru a comunica cu serverul
void GetWeatherData(string& city_name, const string& api_key) {
    CURL* curl;
    CURLcode res;
    string response;  // Aici se va salva raspunsul JSON

    // Initializeaza biblioteca cURL
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if (curl) {
        // Construieste adresa URL completa cu orasul introdus si cheia API
        string url = "https://api.openweathermap.org/data/2.5/weather?q=" + city_name + "&appid=" + api_key + "&units=metric";

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());          // Seteaza URL-ul cererii
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback); // Seteaza functia callback pentru scriere
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);         // Unde se va scrie raspunsul

        // Executa cererea HTTPS
        res = curl_easy_perform(curl);

        // Verifica daca cererea a reusit
        if (res != CURLE_OK) {
            cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << endl;
        }
        else {
            // Daca orasul exista, extrage si afiseaza informatiile
            if (CheckIfCityExits(response)) {
                GetCityData(response, NAME_AND_TEMPERATURE_ITERATOR_OFFSET);
                GetTemperatureData(response, NAME_AND_TEMPERATURE_ITERATOR_OFFSET);
                FeelsLikeTemperatureData(response, TEMPERATURE_FEELS_LIKE_ITERATOR_OFFSET);
                GetDescriptionData(response, DESCRIPTION_ITERATOR_OFFSET);
            }
            else
                cout << "City does not exist";
        }

        // Elibereaza memoria folosita de cURL
        curl_easy_cleanup(curl);
    }
}


// =============================
// FUNCTIE: SpaceAsURLEncoded
// =============================
// inlocuieste toate spatiile dintr-un string cu "%20" pentru ca numele orasului
// sa fie valid intr-un URL. (Ex: "New York" -> "New%20York")
void SpaceAsURLEncoded(string& s) {
    int pos = s.find(' ');
    if (pos == string::npos) {
        return; 
    }
    else
        s.replace(pos, 1, "%20"); // inlocuieste primul spatiu gasit
    SpaceAsURLEncoded(s);         // Apel recursiv pentru restul stringului
}


// =============================
// FUNCTIE: ClearConsole
// =============================
// Curata consola
void ClearConsole() {
    system("cls");
}


// =============================
// FUNCTIA PRINCIPALA: main()
// =============================
// Aici porneste aplicatia. Se afiseaza un meniu simplu care permite
// utilizatorului sa introduca un oras si sa vada vremea curenta
int main() {
    string city_name;
    int menu_option;

    while (true) {
        ClearConsole();
        cout << "======================" << endl;
        cout << "     Weather APP      " << endl;
        cout << "======================" << endl;
        cout << "1. Check weather" << endl;
        cout << "2. Exit" << endl;
        cout << "Choose an option: ";
        cin >> menu_option;

        cin.ignore(); // Curata bufferul (newline ramas de la cin)

        if (menu_option == 1) {
            cout << "Please enter a city: ";
            getline(cin, city_name);

            // inlocuieste spatiile cu %20 pentru URL valid
            SpaceAsURLEncoded(city_name);

            // Obtine si afiseaza datele meteo pentru orasul introdus
            GetWeatherData(city_name, API_KEY);

            cout << endl;
            cout << "Press ENTER to return to menu...";
            cin.get();
        }
        else if (menu_option == 2) {
            cout << "Exiting..." << endl;
            break;
        }
        else {
            cout << "Invalid option, please try again!" << endl;
            cout << "Press ENTER to return to menu...";
            cin.get();
        }
    }

    // Curata resursele cURL inainte de inchidere
    curl_global_cleanup();
    return 0;
}
