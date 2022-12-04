/** @file network.cpp
 *  This program runs a very simple web server, demonstrating how to serve a
 *  static web page and how to use a web link to get the microcontroller to do
 *  something simple. 
 * 
 *  Based on an examples by A. Sinha at 
 *  @c https://github.com/hippyaki/WebServers-on-ESP32-Codes
 * 
 *  @author A. Sinha
 *  @author JR Ridgely
 *  @author Damond Li
 *  @date   2022-Mar-28 Original stuff by Sinha
 *  @date   2022-Nov-04 Modified for ME507 use by Ridgely
 *  @date   2022-Nov-29 Modified for Airheads Glider Project use by Li
 *  @copyright 2022 by the authors, released under the MIT License.
 */

#include <Arduino.h>
#include "PrintStream.h"
#include <WiFi.h>
#include <WebServer.h>
#include <shares.h>
#include <taskshare.h>

// A share containing a boolean flagging the main script to zero the potentiometers
Share<bool> web_calibrate ("Flag to calibrate/zero");

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
                    <br>
                    <form action="/">
                        <input type="text" style="width:150px;height:50px;font-size:20px;">
                        <input type="submit" value="Set Rudder Gain" style="width:250x;height:50px;font-size:20px;">
                    </form>
                    <br>
                    <form action="/">
                        <input type="text" style="width:150px;height:50px;font-size:20px;">
                        <input type="submit" value="Set Elevator Gain" style="width:250x;height:50px;font-size:20px;">
                    </form>
                    <br>
                    <form action="/">
                        <input type="submit" value="Reset Default Gain" style="width:250x;height:50px;font-size:20px;">
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


/** @brief   Switches the state in a FSM when called by the web server.
 *  @details This method alters a shared variable that contains the current state
 *           of a FSM in the controller task located in main.cpp. The state is switched
 *           to 1.
 */
void handle_Activate (void)
{
    tc_state.put(1);

    String toggle_page = "<!DOCTYPE html> <html> <head>\n";
    toggle_page += "<meta http-equiv=\"refresh\" content=\"1; url='/'\" />\n";
    toggle_page += "</head> <body> <p> <a href='/'>Back to main page</a></p>";
    toggle_page += "</body> </html>";

    server.send (200, "text/html", toggle_page); 
}


/** @brief   Switches the state in a FSM when called by the web server.
 *  @details This method alters a shared variable that contains the current state
 *           of a FSM in the controller task located in main.cpp. The state is switched
 *           to 0.
 */
void handle_Deactivate (void)
{
    tc_state.put(0);

    String toggle_page = "<!DOCTYPE html> <html> <head>\n";
    toggle_page += "<meta http-equiv=\"refresh\" content=\"1; url='/'\" />\n";
    toggle_page += "</head> <body> <p> <a href='/'>Back to main page</a></p>";
    toggle_page += "</body> </html>";

    server.send (200, "text/html", toggle_page); 
}


/** @brief   Switches the state in a FSM when called by the web server.
 *  @details This method alters a shared variable that contains the current state
 *           of a FSM in the controller task located in main.cpp. The state is switched
 *           to 1 and a calibrate boolean is set to true whic will be handled in the
 *           controller task.
 */
void handle_Calibrate (void)
{
    web_calibrate.put(1);
    tc_state.put(0);

    String toggle_page = "<!DOCTYPE html> <html> <head>\n";
    toggle_page += "<meta http-equiv=\"refresh\" content=\"1; url='/'\" />\n";
    toggle_page += "</head> <body> <p> <a href='/'>Back to main page</a></p>";
    toggle_page += "</body> </html>";

    server.send (200, "text/html", toggle_page); 
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