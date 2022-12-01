/** @file main.cpp
 *  This program runs a very simple web server, demonstrating how to serve a
 *  static web page and how to use a web link to get the microcontroller to do
 *  something simple. 
 * 
 *  Based on an examples by A. Sinha at 
 *  @c https://github.com/hippyaki/WebServers-on-ESP32-Codes
 * 
 *  @author A. Sinha
 *  @author JR Ridgely
 *  @author D. Li
 *  @date   2022-Mar-28 Original stuff by Sinha
 *  @date   2022-Nov-04 Modified for ME507 use by Ridgely
 *  @date   2022-Nov-29 Modified for Airheads Glider Project use by Li
 *  @copyright 2022 by the authors, released under the MIT License.
 */

#include <Arduino.h>
#include "PrintStream.h"
#include <WiFi.h>
#include <WebServer.h>

// #define USE_LAN to have the ESP32 join an existing Local Area Network or 
// #undef USE_LAN to have the ESP32 act as an access point, forming its own LAN
#undef USE_LAN

// If joining an existing LAN, get certifications from a header file which you
// should NOT push to a public repository of any kind
#ifdef USE_LAN
#include "mycerts.h"       // For access to your WiFi network; see setup_wifi()

// If the ESP32 creates its own access point, put the credentials and network
// parameters here; do not use any personally identifying or sensitive data

#else
const char* ssid = "AirHeads 507";   // SSID, network name seen on LAN lists
const char* password = "??what??";   // ESP32 WiFi password (min. 8 characters)

/* Put IP Address details */
IPAddress local_ip (192, 168, 5, 1); // Address of ESP32 on its own network
IPAddress gateway (192, 168, 5, 1);  // The ESP32 acts as its own gateway
IPAddress subnet (255, 255, 255, 0); // Network mask; just leave this as is
#endif


/// The pin connected to an LED controlled through the Web interface
const uint8_t ledPin = 2;
#define FAST_PIN 12         ///< The GPIO pin cranking out a 500 Hz square wave

/** @brief   The web server object for this project.
 *  @details This server is responsible for responding to HTTP requests from
 *           other computers, replying with useful information.
 *
 *           It's kind of clumsy to have this object as a global, but that's
 *           the way Arduino keeps things simple to program, without the user
 *           having to write custom classes or other intermediate-level 
 *           structures. 
*/
WebServer server (80);


/** @brief   Get the WiFi running so we can serve some web pages.
 */
void setup_wifi (void)
{
#ifdef USE_LAN                           // If connecting to an existing LAN
    Serial << "Connecting to " << ssid;

    // The SSID and password should be kept secret in @c mycerts.h.
    // This file should contain the two lines,
    //   const char* ssid = "YourWiFiNetworkName";
    //   const char* password = "YourWiFiPassword";
    WiFi.begin (ssid, password);

    while (WiFi.status() != WL_CONNECTED) 
    {
        vTaskDelay (1000);
        Serial.print (".");
    }

    Serial << "connected at IP address " << WiFi.localIP () << endl;

#else                                   // If the ESP32 makes its own LAN
    Serial << "Setting up WiFi access point...";
    WiFi.mode (WIFI_AP);
    WiFi.softAPConfig (local_ip, gateway, subnet);
    WiFi.softAP (ssid, password);
    Serial << "done." << endl;
#endif
}


/** @brief   Put a web page header into an HTML string. 
 *  @details This header may be modified if the developer wants some actual
 *           @a style for her or his web page. It is intended to be a common
 *           header (and stylle) for each of the pages served by this server.
 *  @param   a_string A reference to a string to which the header is added; the
 *           string must have been created in each function that calls this one
 *  @param   page_title The title of the page
*/
void HTML_header (String& a_string, const char* page_title)
{
    a_string += R"rawliteral(
        <!DOCTYPE html>
        <html lang="en">
            <head>
                <meta charset="utf-8">
                <meta name="viewport" content="initial-scale=1, width=device-width">
                <title>)rawliteral";
    a_string += page_title;
    a_string += R"rawliteral(
                </title>
                <style>
                    html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align:center;}
                    body { margin-top: 50px;}
                    h1 { color: #4444AA; margin:50px auto 30px;}
                    p { font-size: 24px; color: #222222; margin-bottom:10px;}
                    input { width:250px;height:100px;font-size:20px;}
                </style>
            </head>
            )rawliteral";
}


/** @brief   Callback function that responds to HTTP requests without a subpage
 *           name.
 *  @details When another computer contacts this ESP32 through TCP/IP port 80
 *           (the insecure Web port) with a request for the main web page, this
 *           callback function is run. It sends the main web page's text to the
 *           requesting machine.
 */
void handle_DocumentRoot ()
{
    Serial << "HTTP request from client #" << server.client () << endl;

    String a_str;
    HTML_header (a_str, "ESP32 Web Server Test - Airheads");
    a_str += R"rawliteral(
        <body>
            <main>
                <div id="webpage">
                    <h1>Main Page for ME507 Glider Project</h1>
                    <h2>Control Panel</h2>
                    <table>
                        <tr>
                            <form action="/activate">
                                <input type="submit" value="Activate Flight Control">
                            </form>
                            <form action="/deactivate">
                                <input type="submit" value="Deactivate Flight Control">
                            </form>
                            <form action="/calibrate">
                                <input type="submit" value="Calibrate/Zero">
                            </form>
                        </tr>
                    </table>
                    <h2>
                        Manual Control
                    </h2>
                    <form action="/set_rudder">
                        <input type="text" style="width:150px;height:50px;font-size:20px;">
                        <input type="submit" value="Set Rudder (-90, 90)" style="width:250x;height:50px;font-size:20px;">
                    </form>
                    <br>
                    <form action="/set_elevator">
                        <input type="text" style="width:150px;height:50px;font-size:20px;">
                        <input type="submit" value="Set Elevator (-90, 90)" style="width:250x;height:50px;font-size:20px;">
                    </form>

                </div>
            </main>
        </body>
    </html>
    )rawliteral";

    server.send (200, "text/html", a_str); 
}


/** @brief   Respond to a request for an HTTP page that doesn't exist.
 *  @details This function produces the Error 404, Page Not Found error. 
 */
void handle_NotFound (void)
{
    server.send (404, "text/plain", "Not found");
}

/** @brief   Toggle blue LED when called by the web server.
 *  @details For testing purposes, this function turns the little blue LED on a
 *           38-pin ESP32 board on and off. It is called when someone enters
 *           @c http://server.address/toggle as the web address request from a
 *           browser.
 */
void handle_Activate (void)
{
    static bool state = false;

    digitalWrite (ledPin, state);
    state = !state;

    String toggle_page = "<!DOCTYPE html> <html> <head>\n";
    toggle_page += "<meta http-equiv=\"refresh\" content=\"1; url='/'\" />\n";
    toggle_page += "</head> <body> <p> <a href='/'>Back to main page</a></p>";
    toggle_page += "</body> </html>";

    server.send (200, "text/html", toggle_page); 
}

/** @brief   Toggle blue LED when called by the web server.
 *  @details For testing purposes, this function turns the little blue LED on a
 *           38-pin ESP32 board on and off. It is called when someone enters
 *           @c http://server.address/toggle as the web address request from a
 *           browser.
 */
void handle_Deactivate (void)
{
    static bool state = false;

    digitalWrite (ledPin, state);
    state = !state;

    String toggle_page = "<!DOCTYPE html> <html> <head>\n";
    toggle_page += "<meta http-equiv=\"refresh\" content=\"1; url='/'\" />\n";
    toggle_page += "</head> <body> <p> <a href='/'>Back to main page</a></p>";
    toggle_page += "</body> </html>";

    server.send (200, "text/html", toggle_page); 
}


/** @brief   Show some simulated data when asked by the web server.
 *  @details The contrived data is sent in a relatively efficient Comma
 *           Separated Variable (CSV) format which is easily read by Matlab(tm)
 *           and Python and spreadsheets.
 */
void handle_Calibrate (void)
{
    // The page will be composed in an Arduino String object, then sent.
    // The first line will be column headers so we know what the data is
    String csv_str = "Time, Jumpiness\n";

    // Create some fake data and put it into a String object. We could just
    // as easily have taken values from a data array, if such an array existed
    for (uint8_t index = 0; index < 20; index++)
    {
        csv_str += index;
        csv_str += ",";
        csv_str += String (sin (index / 5.4321), 3);       // 3 decimal places
        csv_str += "\n";
    }

    // Send the CSV file as plain text so it can be easily saved as a file
    server.send (404, "text/plain", csv_str);
}

/** @brief   Show some simulated data when asked by the web server.
 *  @details The contrived data is sent in a relatively efficient Comma
 *           Separated Variable (CSV) format which is easily read by Matlab(tm)
 *           and Python and spreadsheets.
 */
void handle_Set_Rudder (void)
{
    // The page will be composed in an Arduino String object, then sent.
    // The first line will be column headers so we know what the data is
    String csv_str = "Time, Jumpiness\n";

    // Create some fake data and put it into a String object. We could just
    // as easily have taken values from a data array, if such an array existed
    for (uint8_t index = 0; index < 20; index++)
    {
        csv_str += index;
        csv_str += ",";
        csv_str += String (sin (index / 5.4321), 3);       // 3 decimal places
        csv_str += "\n";
    }

    // Send the CSV file as plain text so it can be easily saved as a file
    server.send (404, "text/plain", csv_str);
}

/** @brief   Show some simulated data when asked by the web server.
 *  @details The contrived data is sent in a relatively efficient Comma
 *           Separated Variable (CSV) format which is easily read by Matlab(tm)
 *           and Python and spreadsheets.
 */
void handle_Set_Elevator (void)
{
    // The page will be composed in an Arduino String object, then sent.
    // The first line will be column headers so we know what the data is
    String csv_str = "Time, Jumpiness\n";

    // Create some fake data and put it into a String object. We could just
    // as easily have taken values from a data array, if such an array existed
    for (uint8_t index = 0; index < 20; index++)
    {
        csv_str += index;
        csv_str += ",";
        csv_str += String (sin (index / 5.4321), 3);       // 3 decimal places
        csv_str += "\n";
    }

    // Send the CSV file as plain text so it can be easily saved as a file
    server.send (404, "text/plain", csv_str);
}


/** @brief   Task which sets up and runs a web server.
 *  @details After setup, function @c handleClient() must be run periodically
 *           to check for page requests from web clients. One could run this
 *           task as the lowest priority task with a short or no delay, as there
 *           generally isn't much rush in replying to web queries.
 *  @param   p_params Pointer to unused parameters
 */
void task_webserver (void* p_params)
{
    // The server has been created statically when the program was started and
    // is accessed as a global object because not only this function but also
    // the page handling functions referenced below need access to the server
    server.on ("/", handle_DocumentRoot);
    server.on ("/activate", handle_Activate);
    server.on ("/deactivate", handle_Deactivate);
    server.on ("/calibrate", handle_Calibrate);
    server.onNotFound (handle_NotFound);

    // Get the web server running
    server.begin ();
    Serial.println ("HTTP server started");

    for (;;)
    {
        // The web server must be periodically run to watch for page requests
        server.handleClient ();
        vTaskDelay (500);
    }
}


/** @brief   Task which sends a high frequency signal to show it can run fast.
 *  @details This task sends a square wave signal with a frequency of 500 Hz 
 *           and a duty cyle of 50%. Its purpose in the WiFi demonstration is
 *           to verify that a fast task can run at higher priority than the
 *           WiFi task and keep relatively accurate timing. 
 *  @param   p_params An unused pointer to (no) parameters passed to this task
 */
void task_fast (void* p_params)
{
    Serial << "Start Task Fast" << endl;

    // Configures the pin as an output
    pinMode (FAST_PIN, OUTPUT);

    // Sets the delay for 1 ms
    const TickType_t FAST_DELAY = 1;

    while (true)
    {
        digitalWrite (FAST_PIN, HIGH);
        vTaskDelay (FAST_DELAY);
        digitalWrite(FAST_PIN, LOW);
        vTaskDelay (FAST_DELAY);
    }
}


/** @brief   Arduino setup method which initializes the communication ports and
 *           gets the task(s) running.
 */
void setup () 
{
    Serial.begin (115200);
    delay (100);
    while (!Serial) { }                   // Wait for serial port to be working
    delay (1000);
    Serial << endl << F("\033[2JTesting Arduino Web Server") << endl;

    // Call function which gets the WiFi working
    setup_wifi ();

    // Set up the pin for the blue LED on the ESP32 board
    pinMode (ledPin, OUTPUT);
    digitalWrite (ledPin, LOW);

    // Create the tasks which will do exciting things...

    // Task which runs the web server. It runs at a low priority
    xTaskCreate (task_webserver, "Web Server", 8192, NULL, 2, NULL);

    // Task which produces a square wave (again) at a high priority
    xTaskCreate (task_fast, "500 Hz", 1024, NULL, 5, NULL);
}


/** @brief   Arduino loop method which runs repeatedly, doing nothing much.
 */
void loop ()
{
    vTaskDelay (1000);
}