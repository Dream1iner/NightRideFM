#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h> // For signal handling
#include <unistd.h> // For sleep function
// #include <pthread.h> // For threading
// there was an attempt to update streamTitle...just to let you know

const char *stationUrls[] = {
    "https://stream.nightride.fm/nightride.mp3",
    "https://stream.nightride.fm/chillsynth.mp3",
    "https://stream.nightride.fm/datawave.mp3",
    "https://stream.nightride.fm/spacesynth.mp3",
    "https://stream.nightride.fm/darksynth.mp3",
    "https://stream.nightride.fm/horrorsynth.mp3",
    "https://stream.nightride.fm/ebsm.mp3",
    "https://stream.nightride.fm/chillsynth.mp3",
    "https://stream.nightride.fm/rektory.mp3"
};

const char *stationNames[] = {
    "NightRide", "ChillSynth", "DataWave", "SpaceSynth",
    "DarkSynth", "HorrorSynth", "EBSM", "Archives", "Old stuff"
};

// Indication whether music is playing
int musicPlaying = 0;

// Function to handle SIGINT signal
void sigintHandler(int signum) {
    if (musicPlaying) {
        printf("\nStopping music playback...\n");
        // For now, we'll just print a message indicating that music is stopped.
        musicPlaying = 0;
    }
    // Prompt for input again
    printf("Enter station number (0 to exit): ");
    fflush(stdout); 
}

// Function to play the stream using ffplay
void playStream(const char *url) {
    char command[256];
    snprintf(command, sizeof(command), "ffplay -nodisp -autoexit \"%s\" 2>&1", url); // Redirect stderr to stdout
    FILE *fp = popen(command, "r"); // Open a pipe to the output of ffplay
    if (fp == NULL) {
        printf("Error: Failed to execute ffplay.\n");
        return;
    }
    
    
    // Allocate memory for buffer dynamically
    // this leads to issues with printing streamTitleValue...
    
    /*
    char *buffer = (char *)malloc(4096 * sizeof(char));
    if (buffer == NULL) {
        printf("Failed to allocate memory for buffer.\n");
        pclose(fp);
        return;
    }
    */

    char buffer[64000]; // This is to store ~1 second of 320kbps stream


    char streamTitle[256] = ""; 
    // Read output from ffplay
    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        // Check for StreamTitle and extract its value
        if (strstr(buffer, "StreamTitle") != NULL) {
            char *streamTitleValue = strchr(buffer, ':'); // Find the colon
            if (streamTitleValue != NULL) {
                streamTitleValue++; // Move past the colon
                // Remove leading and trailing spaces
                // Hopefully, those spaces were not part of title...
                char *trimmedValue = streamTitleValue + strspn(streamTitleValue, " \t\r\n");
                char *newline = strchr(trimmedValue, '\n'); // Remove newline character
                if (newline != NULL) *newline = '\0';
                // Copy StreamTitle value
                strcpy(streamTitle, trimmedValue);
                break; 
            }
        }
    }

    // I was not able to get around updating it automatically. 
    // For now you can stop stream and start again to see what track is playing.
    printf("StreamTitle: %s\n", streamTitle);
    fflush(stdout); 

    // Continue reading output from ffplay until end of time :)
    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        // You can add additional processing of ffplay output here if needed
    }

    pclose(fp); // Close the pipe
}

int main() {
    int numStations = sizeof(stationUrls) / sizeof(stationUrls[0]);

    // Set up signal handler for SIGINT (Ctrl+C)
    signal(SIGINT, sigintHandler);

    while (1) {
        // Print station options
        printf("Select a station (0 to exit):\n");
        for (int i = 0; i < numStations; i++) {
            printf("%d. %s\n", i + 1, stationNames[i]);
        }
        printf("0. Exit\n");

        // Get user input
        int selection;
        printf("Enter station number: ");
        if (scanf("%d", &selection) != 1) {
            printf("Error: Invalid input.\n");
            fflush(stdin); // Clear input buffer
            continue;
        }

        // Exit if user inputs 0
        if (selection == 0) {
            printf("Exiting...\n");
            break;
        }

        // Validate user input
        if (selection < 1 || selection > numStations) {
            printf("Invalid selection.\n");
            continue;
        }

        // Play the selected stream
        printf("Playing %s...\n", stationNames[selection - 1]);
        playStream(stationUrls[selection - 1]);
        musicPlaying = 1;

        // Wait for music playback to finish
        // For now, we'll simulate a wait by sleeping for a few seconds.
        sleep(1);
        musicPlaying = 0; // Set musicPlaying to 0 when playback is finished
    }

    return 0;
}
