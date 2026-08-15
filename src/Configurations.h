#pragma once

/**
 * Which directory to scan for?
 * ./samples
 * ../samples
 * /PATH/samples
 * 
 * This value is ignored later, if a parameter was passed.
 */
#define SCANNER_SAMPLES_DIRECTORY "../samples"

/**
 * Set according to your computer's capabilities
 * Loadng many samples will descroty the procssing time.
 */
#define SCANNER_SAMPLES_COUNT_LIMIT 50

/**
 * 100kb limit to touch a file
 */
#define SCANNER_SAMPLES_FILESIZE_LIMIT 102400

/**
 * Pick samples less than 10 seconds
 * Taking longer samples will sqeeeze the wav graph
 */
#define SCANNER_SAMPLES_MAX_PLAYTIME 10.0f