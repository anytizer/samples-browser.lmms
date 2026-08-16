#pragma once

/**
 * Which directory to scan for?
 * ./samples
 * ../samples
 * /PATH/samples?
 * from CLI Parameter?
 * 
 * This value is ignored later, if an argc/argv parameter was passed.
 * Failback default to here.
 */
#define SCANNER_SAMPLES_DIRECTORY "../samples"

/**
 * Set according to your computer's capabilities
 * Loadng too many samples will destroy the procssing time.
 * 
 * Default value: 50
 */
#define SCANNER_SAMPLES_COUNT_LIMIT 150

/**
 * 100kb limit to consider a file
 * 
 * Default value: 102400
 */
#define SCANNER_SAMPLES_FILESIZE_LIMIT 102400

/**
 * Pick samples less than 10 seconds
 * Taking longer samples will:
 *  - sqeeeze the wav graph.
 *  - make the graphs shorter and invisible to compare
 * 
 * Default value: 10.0f
 */
#define SCANNER_SAMPLES_MAX_PLAYTIME 10.0f

/**
 * Width and Height of the wav graph image
 * That appears in the 7th column
 * 
 * Default value: 300 x 50
 */
#define SCANNER_SAMPLES_WAVGRAPH_WIDTH 300
#define SCANNER_SAMPLES_WAVGRAPH_HEIGHT 50

/**
 * Appearance of the columns in the table
 * 
 * DO NOT MODIFY
 */
#define SCANNER_SAMPLES_COLUMN0 0 // file name
#define SCANNER_SAMPLES_COLUMN1 1 // file size
#define SCANNER_SAMPLES_COLUMN2 2 // sample rate
#define SCANNER_SAMPLES_COLUMN3 3 // channnels
#define SCANNER_SAMPLES_COLUMN4 4 // bits / sample
#define SCANNER_SAMPLES_COLUMN5 5 // playtime
#define SCANNER_SAMPLES_COLUMN6 6 // wav graph
