#include <iostream>
#include <fstream>
#include <string>
#include <winsock2.h>
#include <sqlite3.h>
#include <ctime>
#include <chrono>
#include <thread>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "sqlite3.lib")
std::string prefix_p1, prefix_p2;

// SQLite callback function to handle SQL execution results
int callback(void* notUsed, int argc, char** argv, char** columnNames) {
    // Handle the results if needed
    return 0;
}
/// ///////////////
void printSortedData() {
    while (true) {
        // Place your database query code here
        sqlite3* db;
        if (sqlite3_open("mydatabase.db", &db) == SQLITE_OK) {
            std::cout << "SQLite database opened successfully." << std::endl;
            
            // Execute a query to retrieve data sorted by the "data" column in ascending order
            const char* query = "SELECT prefix, data FROM mytable ORDER BY data ASC;";
            sqlite3_stmt* stmt;

            if (sqlite3_prepare_v2(db, query, -1, &stmt, 0) == SQLITE_OK) {
                while (sqlite3_step(stmt) == SQLITE_ROW) {
                    const char* prefix = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
                    int data = sqlite3_column_int(stmt, 1);
                    if(strcmp(prefix, prefix_p1.c_str()) == 0){
                    std::cout << "Data: " << data << std::endl;
                    }
                }
                sqlite3_finalize(stmt);
            } else {
                std::cerr << "Failed to prepare SQL statement." << std::endl;
            }

            sqlite3_close(db);
        } else {
            std::cerr << "Failed to open the SQLite database." << std::endl;
        }

        // Wait for two minutes (2 minutes = 120 seconds)
        std::this_thread::sleep_for(std::chrono::seconds(120));
    }
}
////////////////////////////////////
int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Failed to initialize Winsock." << std::endl;
        return 1;
    }
    ////////////////////
    
    ////////////////////
    // Read configuration from fconfig.ini
    int port = 0;
    

    std::ifstream configFile("fconfig.ini");
    std::string line;
    while (std::getline(configFile, line)) {
        if (line.find("port=") == 0) {
            port = std::stoi(line.substr(5));
            printf("Read port\n");
        } else if (line.find("p1=") == 0) {
            prefix_p1 = line.substr(3);
            printf("Read p1\n");
        } else if (line.find("p2=") == 0) {
            prefix_p2 = line.substr(3);
        }
        printf("Read configuration\n");
    }
    
    configFile.close();

    if (port == 0 || prefix_p1.empty() || prefix_p2.empty()) {
        std::cerr << "Invalid configuration in fconfig.ini." << std::endl;
        WSACleanup();
        return 1;
    }

    // Create and bind the socket
    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }
    ////
    
    ////////
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(static_cast<u_short>(port));

    if (bind(serverSocket, reinterpret_cast<SOCKADDR*>(&serverAddress), sizeof(serverAddress)) == SOCKET_ERROR) {
        std::cerr << "Binding failed: " << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    // Listen for incoming connections
    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Listening failed: " << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    // Initialize SQLite database and open a connection
    sqlite3* db;
    if (sqlite3_open("mydatabase.db", &db) != SQLITE_OK) {
        std::cerr << "Failed to open SQLite database." << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        printf("Initialize SQLite\n");
        return 1;
    }

    // Create a table for storing data if it doesn't exist
    std::string createTableSQL = "CREATE TABLE IF NOT EXISTS mytable (prefix TEXT, data INTEGER, time TEXT)";
    if (sqlite3_exec(db, createTableSQL.c_str(), callback, 0, 0) != SQLITE_OK) {
        std::cerr << "Failed to create table." << std::endl;
        sqlite3_close(db);
        closesocket(serverSocket);
        WSACleanup();
        printf("create table\n");
        return 1;
    }

    // Accept incoming connections and process data
    sockaddr_in clientAddress;
    int clientAddrLen = sizeof(clientAddress);
    std::thread dataThread(printSortedData);
    while (true) {
        SOCKET clientSocket = accept(serverSocket, reinterpret_cast<SOCKADDR*>(&clientAddress), &clientAddrLen);
        if (clientSocket == INVALID_SOCKET) {
            std::cerr << "Acceptance failed: " << WSAGetLastError() << std::endl;
            closesocket(serverSocket);
            sqlite3_close(db);
            WSACleanup();
            return 1;
        }

        char buffer[16384];
        int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);

        if (bytesRead == SOCKET_ERROR) {
            std::cerr << "Receiving data failed: " << WSAGetLastError() << std::endl;
            closesocket(clientSocket);
            continue; // Continue listening for other connections
        }

        std::string receivedData(buffer, bytesRead);
        std::cout << "data is:" << receivedData <<"\n" ;
        std::cout << "prefix is:" << prefix_p2 <<"\n" ;
        // Process received data based on prefixes
        std::string dataToSave,dataforSave;
        if (receivedData.find(prefix_p1) != std::string::npos) {
            dataforSave = receivedData.substr(prefix_p1.length()+1);
            std::time_t t = std::time(0);
            std::tm* now = std::localtime(&t);
            char  timeString[80];
            std::strftime(timeString, 80, "%Y-%m-%d %H:%M:%S", now);
            std::cout << "Current time: " << timeString << std::endl;
            // Concatenate dataToSave and the current time with a semicolon
            dataToSave = dataforSave + ";" + timeString;
            dataToSave = dataToSave.substr(dataforSave.length()+1);
            std:: cout << dataToSave;

            std::string insertDataSQL = "INSERT INTO mytable (prefix, data, time) VALUES (?, ?, ?)";
            sqlite3_stmt* stmt;
            if (sqlite3_prepare_v2(db, insertDataSQL.c_str(), -1, &stmt, 0) == SQLITE_OK) {
                sqlite3_bind_text(stmt, 1, prefix_p1.c_str(), -1, SQLITE_STATIC);
                sqlite3_bind_int(stmt, 2,  std::stoi(dataforSave));
                sqlite3_bind_text(stmt, 3, dataToSave.c_str(), -1, SQLITE_STATIC);
                printf("save the data\n");
                if (sqlite3_step(stmt) != SQLITE_DONE) {
                std::cerr << "Failed to save data to the database." << std::endl;
            }
            sqlite3_finalize(stmt);
        } else {
            std::cerr << "Failed to prepare SQL statement." << std::endl;
        }
        } else if (receivedData.find(prefix_p2) != std::string::npos) {
            dataforSave = receivedData.substr(prefix_p2.length()+1);
            std::time_t t = std::time(0);
            std::tm* now = std::localtime(&t);
            char  timeString[80];
            std::strftime(timeString, 80, "%Y-%m-%d %H:%M:%S", now);
            dataToSave = dataforSave + ";" + timeString;
            dataToSave = dataToSave.substr(dataforSave.length()+1);

            std::string insertDataSQL = "INSERT INTO mytable (prefix, data, time) VALUES (?, ?, ?)";
            sqlite3_stmt* stmt;
            if (sqlite3_prepare_v2(db, insertDataSQL.c_str(), -1, &stmt, 0) == SQLITE_OK) {
                sqlite3_bind_text(stmt, 1, prefix_p2.c_str(), -1, SQLITE_STATIC);
                sqlite3_bind_text(stmt, 2, dataforSave.c_str(), -1, SQLITE_STATIC);
                sqlite3_bind_text(stmt, 3, dataToSave.c_str(), -1, SQLITE_STATIC);
                printf("save the data\n");
                if (sqlite3_step(stmt) != SQLITE_DONE) {
                std::cerr << "Failed to save data to the database." << std::endl;
            }
            sqlite3_finalize(stmt);
        } else {
            std::cerr << "Failed to prepare SQL statement." << std::endl;
        }
        }
        
        // Save data to the SQLite database
        

        //////////////////////////////////////////////////////////
        

        // Close the client socket
        closesocket(clientSocket);
    }

    dataThread.join();
    
    // Close the server socket (this part is not reached in this example)
    closesocket(serverSocket);
    sqlite3_close(db);
    WSACleanup();
    
    return 0;
}
