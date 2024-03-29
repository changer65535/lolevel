#ifndef __GRADEDATA__
#define __GRADEDATA__
struct gradeDataType
{
  float dist;
  int grade;
  
};
  ///////////////////LANDMARK DATA////////////////////
struct landmarkDataType
{
  float dist;
  char landmark[32];
};
  #define MAX_HILLY 190
const gradeDataType PROGMEM hillyRun[MAX_HILLY] = {{0,1},{0.03,0},{0.34,1},{0.43,2},{0.46,4},{0.47,2},{0.48,1},{0.51,0},{0.57,1},{0.63,4},{0.65,3},{0.66,4},
{0.67,2},{0.69,1},{0.7,0},{0.72,6},{0.74,7},{0.75,8},{0.77,9},{0.78,8},{0.79,4},{0.8,10},{0.81,10},{0.82,9},{0.83,8},{0.86,7},{0.94,8},{0.95,0},
{0.98,-2},{0.99,0},{1,2},{1.01,6},{1.02,7},{1.17,7},{1.18,0},{1.19,-1},{1.2,2},{1.21,4},{1.22,5},{1.23,4},{1.25,3},{1.26,2},{1.27,1},{1.28,0},
{1.29,-1},{1.45,0},{1.46,1},{1.47,2},{1.48,1},{1.63,0},{1.64,-1},{1.66,-2},{1.73,-1},{1.75,0},{1.76,1},{1.8,0},{1.82,-1},{1.84,-2},{1.85,-5},
{1.91,-4},{1.99,0},{2,4},{2.01,6},{2.04,4},{2.05,5},{2.07,4},{2.08,2},{2.09,1},{2.1,0},{2.11,-1},{2.15,-2},{2.18,-1},{2.2,-3},{2.26,-1},{2.29,-1},
{2.34,-2},{2.36,-4},{2.37,-6},{2.42,2},{2.43,3},{2.44,3},{2.45,3},{2.46,-2},{2.47,-4},{2.89,0},{2.91,1},{2.97,0},{3.1,-1},{3.18,1},{3.19,3},{3.2,3},
{3.21,3},{3.23,4},{3.24,4},{3.3,3},{3.31,3},{3.32,2},{3.33,1},{3.34,-1},{3.36,1},{3.37,2},{3.38,4},{3.39,5},{3.41,5},{3.42,4},{3.43,2},{3.46,0},
{3.47,0},{3.48,-1},{3.5,-2},{3.52,-3},{3.54,-4},{3.6,-2},{3.66,0},{3.67,0},{3.68,-1},{3.69,-5},{3.71,-4},{3.75,0},{3.76,1},{3.77,2},{3.78,1},
{3.86,0},{3.93,-1},{4,-1},{4.04,0},{4.06,1},{4.14,0},{4.16,-1},{4.19,0},{4.31,-1},{4.33,-2},{4.35,6},{4.45,5},{4.49,4},{4.5,3},{4.52,-1},{4.53,-2},
{4.55,-3},{4.68,1},{4.69,2},{4.71,3},{4.73,4},{4.74,3},{4.76,2},{4.77,1},{4.78,-11},{4.79,5},{4.81,-3},{4.82,-3},{4.83,-2},{4.85,0},{4.86,1},
{4.87,-1},{4.9,-4},{4.95,6},{4.96,7},{4.97,-4},{4.99,1},{5,-1},{5.06,4},{5.07,5},{5.09,3},{5.12,2},{5.13,2},{5.14,-1},{5.15,1},{5.18,0},{5.19,-1},
{5.2,-4},{5.21,-1},{5.22,-3},{5.23,-3},{5.24,-2},{5.25,-2},{5.26,1},{5.27,2},{5.28,1},{5.29,0},{5.3,-2},{5.31,-2},{5.32,-3},{5.33,-3},{5.34,-3},
{5.44,-4},{5.49,-2},{5.59,-1},{5.63,0},{5.64,0},
{-1,-1}};

#define MAX_VOLCANO 124
const gradeDataType PROGMEM volcanoClimb[MAX_VOLCANO] = {{0,0},{0.29,-1},{0.36,0},{0.73,-1},{0.75,0},{0.76,5},{0.77,6},{0.78,5},{0.79,4},
{0.8,2},{0.81,0},{0.82,-1},{0.85,1},{0.87,-1},{0.9,-5},{0.93,1},{0.94,1},{0.96,0},{1.05,-1},{1.09,0},{1.1,0},{1.12,1},{1.13,2},{1.15,3},
{1.17,4},{1.18,3},{1.19,4},{1.2,5},{1.22,4},{1.23,0},{1.24,-5},{1.28,-1},{1.29,3},{1.3,4},{1.31,5},{1.48,4},{1.52,5},{1.67,4},{1.73,5},
{1.78,4},{1.79,3},{1.8,2},{1.81,0},{1.82,-2},{1.83,-3},{1.85,0},{1.86,5},{1.87,5},{1.88,6},{1.89,5},{1.9,4},{1.91,3},{1.92,3},{1.93,4},
{1.95,4},{1.96,4},{1.97,4},{1.98,3},{1.99,2},{2,1},{2.01,0},{2.02,-2},{2.03,-6},{2.04,-7},{2.05,-5},{2.06,-5},{2.07,1},{2.08,2},{2.09,6},
{2.12,5},{2.13,2},{2.14,0},{2.15,-1},{2.17,0},{2.18,0},{2.32,2},{2.33,3},{2.39,2},{2.42,3},{2.44,4},{2.47,3},{2.51,4},{2.53,5},{2.79,8},
{2.83,5},{2.84,4},{2.85,-1},{2.87,2},{2.88,3},{2.92,2},{2.93,3},{2.95,4},{3.05,5},{3.06,4},{3.08,3},{3.1,4},{3.15,5},{3.18,4},{3.2,2},
{3.21,1},{3.24,2},{3.25,3},{3.3,4},{3.34,5},{3.38,4},{3.41,5},{3.42,6},{3.43,8},{3.44,10},{3.49,8},{3.5,6},{3.51,4},{3.52,1},{3.53,0},
{3.54,-1},{3.55,-2},{3.57,0},{3.8,-1},{3.83,-3},{3.84,-4},
{-1,-1}};

#define MAX_ELEVEN 130
const gradeDataType PROGMEM elevenOcean[MAX_ELEVEN] = {{0,2},{0.08,1},{0.14,0},{0.19,2},{0.2,3},{0.24,2},{0.25,0},{0.26,-2},{0.27,-3},
{0.31,0},{0.36,1},{0.38,2},{0.39,1},{0.4,0},{0.54,-1},{0.55,-2},{0.56,-4},{0.61,0},{0.62,2},{0.63,3},{0.66,4},{0.73,3},{0.74,2},
{0.75,0},{0.76,2},{0.77,1},{0.78,0},{0.79,-1},{1,-2},{1.01,-1},{1.76,0},{1.88,-1},{1.92,1},{1.95,0},{2.06,1},{2.08,0},{2.43,1},
{2.44,2},{2.45,-1},{2.46,-2},{2.47,-3},{2.48,-4},{2.59,-3},{2.6,-2},{2.61,-1},{2.62,1},{2.68,0},{3.63,-1},{3.69,0},{3.7,1},{3.72,2},
{3.73,4},{3.84,2},{3.85,1},{3.86,0},{3.87,-1},{3.88,-1},{4.07,-1},{4.08,0},{4.1,1},{4.12,2},{4.16,3},{4.21,2},{4.22,1},{4.3,-1},{4.33,-2},
{4.35,-3},{4.36,-2},{4.38,-1},{4.4,-3},{4.42,-1},{4.44,0},{4.49,-1},{4.52,0},{4.53,1},{4.62,0},{4.69,-1},{4.82,0},{4.83,1},{4.85,0},
{4.92,-1},{4.97,0},{4.99,1},{5.1,2},{5.11,3},{5.18,2},{5.19,-1},{5.2,-3},{5.3,-2},{5.32,-1},{5.37,0},{5.38,1},{5.41,2},{5.44,1},{5.48,2},
{5.64,1},{5.7,2},{5.71,1},{5.75,2},{6.06,1},{6.1,2},{6.15,1},{6.17,0},{6.2,-1},{6.21,-2},{6.22,-3},{6.25,-2},{6.3,-3},{6.32,-4},{6.35,-3},
{6.37,0},{6.38,1},{6.4,0},{6.46,1},{6.52,-1},{6.61,-2},{6.63,-1},{6.64,0},{6.65,1},{6.69,-1},{6.73,3},{6.77,2},{6.78,1},{6.79,0},{6.8,-2},
{6.81,-3},{6.84,-2},{6.85,0},{6.86,-1},
{-1,-1}};

#define MAX_MOUNTAIN 283
const gradeDataType PROGMEM mountainRoute[MAX_MOUNTAIN] = {{0,0},{0.6,1},{0.69,2},{0.71,3},{0.72,4},{0.73,2},{0.74,1},{0.78,-1},{0.86,-2},
{0.87,-3},{0.91,-2},{0.95,-1},{0.99,1},{1,0},{1.17,1},{1.18,2},{1.19,1},{1.2,-1},{1.21,-2},{1.22,-3},{1.23,-4},{1.33,-3},{1.34,-2},{1.35,-1},
{1.38,1},{2.59,-1},{2.6,-2},{2.61,-3},{2.62,0},{2.63,3},{2.64,4},{2.65,6},{2.67,7},{2.68,6},{2.69,7},{2.7,6},{2.77,2},{2.79,0},{2.8,-1},
{2.81,-2},{2.82,-1},{2.84,0},{3.09,1},{3.12,2},{3.14,3},{3.15,5},{3.16,10},{3.17,9},{3.19,8},{3.22,7},{3.23,6},{3.27,7},{3.28,6},{3.31,7},
{3.36,8},{3.37,7},{3.38,6},{3.41,7},{3.42,8},{3.44,9},{3.45,10},{3.46,9},{3.47,6},{3.48,7},{3.49,8},{3.5,10},{3.51,8},{3.53,7},{3.55,6},
{3.56,5},{3.57,4},{3.61,3},{4.16,3},{4.17,4},{4.22,5},{4.24,6},{4.26,7},{4.31,6},{4.32,7},{4.34,6},{4.35,7},{4.36,6},{4.37,4},{4.38,2},
{4.39,1},{4.4,2},{4.41,4},{4.42,5},{4.49,6},{4.55,5},{4.57,6},{4.69,5},{4.71,6},{4.72,5},{4.78,3},{4.79,2},{4.82,4},{4.83,3},{4.85,4},
{4.86,7},{4.88,6},{4.94,7},{5,6},{5.07,7},{5.09,6},{5.11,7},{5.14,6},{5.15,5},{5.17,6},{5.47,5},{5.51,6},{5.54,7},{5.61,6},{5.62,5},{5.63,3},
{5.64,2},{5.66,0},{5.81,2},{5.82,3},{5.83,4},{5.84,6},{5.85,8},{5.86,7},{5.87,8},{5.88,7},{5.91,6},{5.94,5},{5.95,4},{5.96,3},{5.98,2},{6.02,3},
{6.12,4},{6.13,5},{6.14,6},{6.16,7},{6.3,6},{6.32,4},{6.33,2},{6.36,3},{6.37,4},{6.39,5},{6.41,6},{6.43,5},{6.45,4},{6.48,3},{6.52,4},{6.68,5},
{6.69,6},{6.74,7},{6.8,8},{6.83,7},{6.85,6},{6.92,5},{6.94,4},{6.95,5},{6.98,6},{7.08,7},{7.14,5},{7.15,4},{7.16,2},{7.17,-1},{7.18,-4},{7.2,-5},
{7.21,-6},{7.24,-5},{7.25,-4},{7.27,0},{7.3,1},{7.39,2},{7.45,3},{7.47,4},{7.49,5},{7.5,7},{7.51,8},{7.52,7},{7.55,6},{7.59,7},{7.6,6},{7.61,5},
{7.62,2},{7.63,-6},{7.64,-12},{7.65,-14},{7.66,-13},{7.67,-9},{7.69,1},{7.7,7},{7.71,11},{7.72,6},{7.73,10},{7.74,-6},{7.75,-8},{7.76,-12},
{7.77,0},{7.78,4},{7.79,2},{7.8,5},{7.83,6},{7.84,7},{7.93,6},{7.96,5},{7.97,4},{7.98,3},{7.99,2},{8,1},{8.01,0},{8.04,-1},{8.06,0},{8.07,1},
{8.09,0},{8.1,-1},{8.11,-2},{8.12,-3},{8.13,-5},{8.14,-6},{8.16,-7},{8.18,-8},{8.21,-9},{8.26,-10},{8.29,-9},{8.3,-8},{8.31,-7},{8.33,-5},
{8.34,-3},{8.35,-1},{8.36,1},{8.37,2},{8.38,5},{8.4,6},{8.41,7},{8.45,6},{8.47,7},{8.53,8},{8.54,7},{8.63,6},{8.79,5},{8.83,6},{8.92,5},
{8.95,4},{8.96,3},{8.98,2},{9,1},{9.06,0},{9.16,1},{9.18,-2},{9.19,-10},{9.2,-12},{9.22,-10},{9.23,-7},{9.24,-1},{9.25,4},{9.26,12},{9.27,14},
{9.28,13},{9.33,14},{9.39,13},{9.4,14},{9.45,15},{9.49,14},{9.5,13},{9.51,12},{9.53,13},{9.55,15},{9.56,16},{9.57,15},{9.58,14},{9.59,13},
{9.63,11},{9.65,13},{9.66,14},{9.69,13},{9.81,12},{9.82,11},{9.83,12},{9.9,11},{9.91,9},{9.92,5},{9.93,2},{9.94,0},{9.95,1},{9.97,0},{9.99,-1},
{-1,-1}};

#define MAX_5K 60
const gradeDataType PROGMEM fiveKRoute[MAX_5K] = {{0.01,-2},{0.11,-2},{0.13,-1},{0.14,-2},{0.2,-2},{0.33,0},{0.52,5},{0.56,-5},{0.57,-8},{0.58,0},
{0.63,-1},{0.67,1},{0.85,-1},{0.98,1},{1.07,-1},{1.13,1},{1.23,2},{1.24,1},{1.25,2},{1.27,3},{1.33,2},{1.34,1},{1.35,-3},{1.45,-2},{1.48,-1},{1.53,1},
{1.57,2},{1.59,1},{1.63,2},{1.79,1},{1.85,2},{1.86,1},{1.91,2},{2.21,1},{2.25,2},{2.3,1},{2.35,-1},{2.36,-2},{2.37,-3},{2.4,-2},{2.43,-3},{2.44,-2},
{2.45,-3},{2.47,-4},{2.5,-3},{2.52,-2},{2.53,-1},{2.54,1},{2.61,1},{2.67,-1},{2.76,-2},{2.78,-1},{2.79,0},{2.8,1},{2.88,3},{2.94,-1},{2.95,-3},
{3.06,-1},{3.08,-1}};


  #define HILLY_LANDMARKS 4
  const landmarkDataType PROGMEM hillyRunLandmarks[HILLY_LANDMARKS] = 
  {
    {0.14,"START FINISH"},
    {0.55,"UNDER BRIDGE"},
    {0.85,"ALLEZ"},
    {0,""}};
    
  #define VOLCANO_LANDMARKS 12
  const landmarkDataType PROGMEM volcanoClimbLandmarks[VOLCANO_LANDMARKS] = {
    {0.16,"Tunnel End"},
    {0.51,"Lava Entrance"},
    {0.83,"OutSide Again"},
    {1.12,"KOM Ring"},
    {2.04,"KOM Ring2"},
    {2.44,"Outside Again"},
    {2.84,"Square Rock"},
    {3.1,"KOM Ring3"},
    {3.39,"Square Rock2"},
    {3.52,"KOM TOP"},
    {0,""}};
    
  #define ELEVEN_LANDMARKS 11
  const landmarkDataType PROGMEM elevenOceanLandmarks[ELEVEN_LANDMARKS] = {
    {0.11,"Start Finish"},
    {0.83,"Ocean Blvd Stone Sign"},
    {1.97,"Dock After Blue House"},
    {2.86,"Sequoyah"},
    {3.88,"Last Ring"},
    {4.46,"1KM"},
    {5.08,"2 KM"},
    {5.66,"3 KM"},
    {6.3,"4 KM"},
    {6.92,"5 KM"},
    {0,""}};

    #define MOUNTAIN_ROUTE_LANDMARKS 11
    const landmarkDataType PROGMEM mountainRouteLandmarks[MOUNTAIN_ROUTE_LANDMARKS] = {
      {0.4,"Start Finish"},
      {0.8,"Ocean Blvd"},
      {2.18,"Left Turn"},
      {3.14,"Lights In Road"},
      {4.29,"Stone Sign"},
      {5.22,"BridgePass Sign"},
      {5.79,"Out of Castle"},
      {6.74,"Tunnel Start"},
      {7.38,"Pass 2.98 Km Sign"},
      {8.98,"KOM Finish"},
      {0,""}
    };
    #define FIVE_K_LANDMARKS 9
    const landmarkDataType PROGMEM fiveKLandmarks[FIVE_K_LANDMARKS] = {
      {0.02,"Gap In Signs"},
      {0.21,"Tunnel Beginning"},
      {0.55,"Top Of Ramp"},
      {0.9,"Run Thru Net"},
      {0.89,"1M"},
      {1.23,"2K"},
      {1.92,"2M"},
      {2.45,"4K"},
      {3.07,"5K"}};
    
    
#endif
 
