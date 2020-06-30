//Written by Theo Watson - http://theowatson.com
//Work in progress class for doing opengl strips / ribbons 

// Edited and improved by Ben Snell

#pragma once
#include "ofMain.h"

class ofxStrip{

public:
	
    ofxStrip();
    void clear();
		
    void generate( vector <ofPoint>& pts, float fixedWidth, ofPoint upVec);
    void generateWithTaper( vector <ofPoint>& pts, float fixedWidth, float taperPct, ofPoint upVec);
    void generateWithTaper( vector <ofPoint>& pts, vector<ofVec2f> widths, float taperPct, ofPoint upVec);
    void generateWithTaper( vector <ofPoint>& pts, vector<ofVec2f> widths, int numPts, ofPoint upVec);
    void generate( vector <ofPoint>& pts, vector <float> width, ofPoint upVec);
    void generate( vector <ofPoint>& pts, vector <ofVec2f> width, ofPoint upVec);
    
    void setColors( vector<ofFloatColor>& colors, bool bSingleColor=true );
    
    void draw();
    void draw(int aBeginIndex, int aCount );
    
    void setTexCoordScale( float tex_u_scale, float tex_v_scale );
    void setTexCoordRange( float aTexXStart, float aTexXEnd, float aTexYStart, float aTexYEnd );
    void enableTexCoords();
    void disableTexCoords();

    void enableNormals();
    void disableNormals();
		
    ofMesh getMesh();
    ofMesh& getMeshRef();

    float tex_u, tex_v;
    float stex_u, stex_v;
    bool bTexCoords; 
    bool bNormals; 
    bool bFlipTex;
    bool bClampTexCoordsToEdge;
    
    ofMesh mesh;
};




