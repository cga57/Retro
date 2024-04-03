/**
 * Created using functions provided in the Bluetooth document provided by Dr. Brian Fraser
 * https://opencoursehub.cs.sfu.ca/bfraser/grav-cms/cmpt433/links/files/2022-student-howtos-ensc351/Bluetooth.pdf
 *
 **/

// Assume device already detectable to other devices
// sudo hciconfig hci0 piscan
// Assume MAC address of client and server known
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <bluetooth/rfcomm.h>
#include <pthread.h>
#include <stdbool.h>
#include "bluetooth.h"

// Initialize thread IDs
static pthread_t deviceDiscoveryId;
static pthread_mutex_t mutex;

// Initialize module variables
static bdaddr_t bluetoothAdapterID = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

// client MAC address
char userIDString[17] = "01:23:45:67:89:YZ"; // server MAC address
static bool userInRange = false;
static bool isDoneRunning = false;

// Initialize data transmission variables
static bool commInProgress = false;
static struct sockaddr_rc adapter_addr = {0}, user_addr = {0};
static int bluetoothSocket = 0;
static int client = 0;

static bool BluetoothServer_isUserInRange(void);
static int discoverDevice(void);
static void *deviceDiscoveryThread(void *_);
static void bluetoothCommInit(void);
static void bluetoothCommRead(void);
static void bluetoothCommCleanup(void);

// Initializes the Bluetooth module and starts thread. Must be run before
// other functions.
void BluetoothServer_init(void)
{
    pthread_create(&deviceDiscoveryId, NULL, deviceDiscoveryThread, NULL);
    return;
}
// Shuts downBlueTooth module and closes thread
void BluetoothServer_cleanup(void)
{
    isDoneRunning = true;
    pthread_join(deviceDiscoveryId, NULL);
    return;
}

// Checks if the user's device is within range
static bool BluetoothServer_isUserInRange(void)
{
    bool status = false;
    pthread_mutex_lock(&mutex);
    status = userInRange;
    pthread_mutex_unlock(&mutex);
    return status;
}

// This function looks up the Bluetooth device name from the known device
// address.
// If successful, function returns 0. If unsuccessful, function returns -1;
static int discoverDevice(void)
{
    int dev_id = 0;
    dev_id = hci_get_route(&bluetoothAdapterID);
    // Open a Bluetooth socket to connect BBG to the Bluetooth adapter
    int sock = 0;
    sock = hci_open_dev(dev_id);
    // Error checking
    if (dev_id < 0 || sock < 0)
    {
        perror("opening socket");
        exit(1);
    }
    char name[248] = {0};
    bdaddr_t userID;
    str2ba(userIDString, &userID);
    // Read name from device ID
    int rsp_code = hci_read_remote_name(sock, &userID, sizeof(name), name, 0);
    close(sock);
    return rsp_code;
}

// This thread continuously scans if the user is within range every 1 sec
static void *deviceDiscoveryThread(void *_)
{
    int scanPeriod = 1; // s
    while (!isDoneRunning)
    {
        int rsp_code = 0;
        rsp_code = discoverDevice();
        if (rsp_code < 0)
        {
            pthread_mutex_lock(&mutex);
            userInRange = false;
            pthread_mutex_unlock(&mutex);
            // If a bluetoothCommThread exists, destroy thread
            if (commInProgress)
            {
                commInProgress = false;
                bluetoothCommCleanup();
            }
        }
        else
        {
            pthread_mutex_lock(&mutex);
            userInRange = true;
            pthread_mutex_unlock(&mutex);
            // If communication is not in progress, initialise new socket
            // connection
            if (!commInProgress)
            {
                bluetoothCommInit();
            }
            if (client > 0)
            {
                commInProgress = true;
                bluetoothCommRead();
            }
        }
        sleep(scanPeriod);
    }
    return NULL;
}

// Connects BlueTooth to user's device
static void bluetoothCommInit(void)
{
    // Allocate socket for Bluetooth communication with RFCOMM protocol
    bluetoothSocket = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
    // Initialise RFCOMM socket for the Bluetooth adapter a
    adapter_addr.rc_family = AF_BLUETOOTH;
    // Since there is only one Bluetooth adapter, any Bluetooth adapter
    // address works
    adapter_addr.rc_bdaddr = *BDADDR_ANY;
    adapter_addr.rc_channel = (uint8_t)1;
    // Bind Bluetooth adapter socket to port 1 (allocate port 1 to the
    // bluetoothSocket)
    bind(bluetoothSocket, (struct sockaddr *)&adapter_addr,
         sizeof(adapter_addr));
    // Put bluetoothSocket into listening mode, allow only 1 connection
    listen(bluetoothSocket, 1);
    printf("Listen for a connection...................................\n");
    // Accept one client connection
    socklen_t usaddr_len = sizeof(user_addr);
    // Blocking call that waits for incoming connection request
    client = accept(bluetoothSocket, (struct sockaddr *)&user_addr,
                    &usaddr_len);
    char buffer[1024] = {0};
    ba2str(&user_addr.rc_bdaddr, buffer);
    fprintf(stderr, "Accepted connection from %s\n", buffer);
    memset(buffer, 0, sizeof(buffer));
    return;
}

// This function continuously read data
static void bluetoothCommRead(void)
{
    int numBytesRead = 0;
    char buffer[1024] = {0};
    // Read data into a buffer
    numBytesRead = read(client, buffer, sizeof(buffer));
    // Parse data from the buffer
    if (numBytesRead > 0)
    {
        // Display transmitted data
        printf("User’s device says %s.\n", buffer);
    }
}

// Closes the BlueTooth socket
static void bluetoothCommCleanup(void)
{
    // Close connection
    close(client);
    close(bluetoothSocket);
}

int main()
{
    // program runs for two minutes
    BluetoothServer_init();
    sleep(120);
    BluetoothServer_cleanup();
    return 0;
}
