#include "ofxStrip.h"


//--------------------------------------------------------------
ofxStrip::ofxStrip(){
    enableTexCoords();
    enableNormals();
    
    setTexCoordRange( 0.f, 1.f, 0.f, 1.f );
    
    bFlipTex    = false;
    bClampTexCoordsToEdge = false;
}

//--------------------------------------------------------------
void ofxStrip::clear(){
    mesh.clear();
}

//--------------------------------------------------------------
void ofxStrip::generate( vector <ofPoint>& pts, float fixedWidth, ofPoint upVec){
    vector <ofVec2f> width;
    width.push_back(ofVec2f(fixedWidth, fixedWidth));
    generate( pts, width, upVec);
}

//--------------------------------------------------------------
void ofxStrip::generateWithTaper( vector <ofPoint>& pts, float fixedWidth, float taperPct, ofPoint upVec){
    vector <ofVec2f> width;
    
    float total = pts.size();
    int num = total * taperPct;
    
    float curW = 0.0;
//    float tarW = fixedWidth;
    float incr = fixedWidth / num ;
    
    width.resize( pts.size(), ofVec2f(0,0) );
    
    for(int i = 0; i < pts.size(); i++){
//        width.push_back(curW);
        width[i].x = curW;
        width[i].y = curW;
        if( i < num ){
            curW += incr;
            curW = ofClamp(curW, 0, fixedWidth);
        }else if( i >= (total - num) ){
            curW -= incr;
            curW = ofClamp(curW, 0, fixedWidth);
        }
    }
    
    generate( pts, width, upVec);
}

//--------------------------------------------------------------
void ofxStrip::generateWithTaper( vector <ofPoint>& pts, vector<ofVec2f> widths, float taperPct, ofPoint upVec){
    taperPct = ofClamp(taperPct, 0.f, .5f);
    
    for(int i = 0; i < pts.size(); i++) {
        float pct = (float)i/(float)(pts.size()-1);
        if(pct <= taperPct) {
            pct = ofMap(pct, 0.f, taperPct, 0.f, 1.f, true);
        } else if (pct >= 1.f - taperPct) {
            pct = ofMap(pct, 1.f-taperPct, taperPct, 1.f, 0.f, true);
        } else {
            pct = 1.f;
        }
        //        width.push_back(curW);
        widths[i].x = widths[i].x * pct;
        widths[i].y = widths[i].y * pct;
    }
    
    generate( pts, widths, upVec);
}

//--------------------------------------------------------------
void ofxStrip::generateWithTaper( vector <ofPoint>& pts, vector<ofVec2f> widths, int numPts, ofPoint upVec){
    
    numPts = ofClamp(numPts, 0.f, pts.size());
    
    for(int i = 0; i < pts.size(); i++) {
        float pct = 1.f;
        if(i <= numPts) {
            pct = ofMap(i, 0.f, numPts, 0.f, 1.f, true);
        } else if (i >= (pts.size()-1) - numPts) {
            pct = ofMap(i, (pts.size()-1)-numPts, pts.size()-1, 1.f, 0.f, true);
        }
//        pct = powf(pct, .75);
//        pct = pct * pct;
        
        widths[i].x = widths[i].x * pct;
        widths[i].y = widths[i].y * pct;
    }
    
    generate( pts, widths, upVec);
}

//--------------------------------------------------------------
void ofxStrip::generate( vector <ofPoint>& pts, vector <float> widths, ofPoint upVec) {
    vector<ofVec2f> twidths;
    twidths.resize(widths.size());
    for(int i = 0; i < twidths.size(); i++ ) {
        twidths[i].x = widths[i];
        twidths[i].y = widths[i];
    }
    generate( pts, twidths, upVec);
}

//--------------------------------------------------------------
void ofxStrip::generate( vector <ofPoint>& pts, vector <ofVec2f> widths, ofPoint upVec) {

    bool  bFixedWidth = false;
    bool  iszAxis = (upVec == ofPoint(0, 0, 1));
    
    ofVec2f curWidth;
    float maxWidth = 0;
    
    float t = 0.0;
    
    if( widths.size() == 1 || ( widths.size() && widths.size() != pts.size() ) ){
        bFixedWidth = true;
        curWidth.x  = widths[0].x;
        curWidth.y  = widths[0].y;
        maxWidth  = MAX(curWidth.x, curWidth.y);
    }
    
    if( !bFixedWidth ){
        for(int i = 0; i < widths.size(); i++){
            if( maxWidth < widths[i].x ){
                maxWidth = widths[i].x;
            }
            if( maxWidth < widths[i].y ){
                maxWidth = widths[i].y;
            }
        }
    }
    
//    mesh.clear();
    
    int totalVerts = pts.size() * 2;
    if(mesh.getNumVertices() != totalVerts) {
        mesh.getVertices().resize( totalVerts );
    }
    if(bNormals) {
        if(mesh.getNumNormals() != totalVerts) {
            mesh.getNormals().resize( totalVerts );
        }
    }
    if(bTexCoords) {
        if(mesh.getNumTexCoords() != totalVerts) {
//            cout << " resizing tex coords " << totalVerts << endl;
            mesh.getTexCoords().resize( totalVerts );
        }
    }
//    else{
//            cout << " tex coords disabled " << endl;
//    }
    
    mesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
    
    int index = 0;
    
    float numPts = pts.size()-1;
    
    ofVec3f toTheLeft;
    ofPoint delta;
    ofPoint deltaNorm;
    
    ofVec2f tcoord1, tcoord2;
    
    for(int i = 0; i < pts.size(); i++){
        
        if( !bFixedWidth ){
            curWidth.x = widths[i].x;
            curWidth.y = widths[i].y;
        }
        
        //find this point and the next point
        int pre, nex;
        
        if( i == 0 ){
            pre = i;
            nex = i+1;
        }else if( i == numPts ){
            pre = i-1;
            nex = i;
        }else{
            pre = i-1;
            nex = i+1;
        }
        
        ofVec3f & thisPoint = pts[i];
        ofVec3f & prePoint  = pts[pre];
        ofVec3f & nextPoint = pts[nex];
        
        delta       = nextPoint - prePoint;
        deltaNorm	= delta.getNormalized();
        
        if( iszAxis ){
            toTheLeft.set(deltaNorm.y, -deltaNorm.x);
        }else{
            toTheLeft = deltaNorm.getPerpendicular(upVec);
        }
        
        ofPoint L = thisPoint + toTheLeft * curWidth.x;
        ofPoint R = thisPoint - toTheLeft * curWidth.y;
        
        mesh.setVertex(index, L);
        mesh.setVertex(index+1, R);
//        mesh.addVertex(L);
//        mesh.addVertex(R);
        
        if( bNormals ){
            ofPoint normal = deltaNorm.getPerpendicular(-toTheLeft);
            mesh.setNormal(index, normal);
            mesh.setNormal(index+1, normal);
//            mesh.addNormal(normal);
//            mesh.addNormal(normal);
        }
        
        if( bTexCoords ){
            float texUPct1 = curWidth.x / maxWidth;
            float texUPct2 = curWidth.y / maxWidth;
            
            float texVPct = t + ( (float)i / numPts);
            if( bFlipTex ){
                texVPct = 1.0-texVPct;
            }
            
            if(bClampTexCoordsToEdge) {
                texUPct1 = 1.f;
                texUPct2 = 1.f;
            }
            
            tcoord1.x = ofMap( 1.f-texUPct1, 0.f, 1.f, stex_u, tex_u, true );
            tcoord1.y = ofMap( texVPct, 0.f, 1.f, stex_v, tex_v, true );
            
            tcoord2.x = ofMap( texUPct2, 0.f, 1.f, stex_u, tex_u, true );
            tcoord2.y = ofMap( texVPct, 0.f, 1.f, stex_v, tex_v, true );
            
            mesh.setTexCoord(index, tcoord1 );
            mesh.setTexCoord(index+1, tcoord2 );
//            mesh.setTexCoord(index, ofVec2f((1.0-texUPct1) * tex_u, texVPct * tex_v));
//            mesh.setTexCoord(index+1, ofVec2f(texUPct2 * tex_u, texVPct * tex_v));
            
        }
//        else{
//            cout << " not setting texcoord " << endl;
//        }
        
        index += 2;
    }
    
//    mesh.setupIndicesAuto();
    
}

//--------------------------------------------------------------
void ofxStrip::setColors( vector<ofFloatColor>& colors, bool bSingleColor ) {
    int tverts = mesh.getNumVertices();
    
    if(mesh.getNumColors() != mesh.getNumVertices()) {
        mesh.getColors().resize( mesh.getNumVertices() );
    }
    
    for(int i = 0; i < colors.size(); i++ ) {
        if(bSingleColor) {
            if(i*2+1 > tverts) break;
            mesh.setColor( i*2+0, colors[i] );
            mesh.setColor( i*2+1, colors[i] );
        } else {
            if(i >= tverts) break;
            mesh.setColor( i, colors[i] );
        }
    }
}

//--------------------------------------------------------------
void ofxStrip::draw() {
    if(mesh.getNumVertices() > 0) {
        mesh.draw();
    } else {
//        cout << "mesh verts < 1" << endl;
    }
}

//--------------------------------------------------------------
void ofxStrip::draw( int aBeginIndex, int aCount ) {
    if(mesh.getNumVertices()) {
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_FLOAT, sizeof(ofVec3f), &mesh.getVerticesPointer()->x);
	}
	if(mesh.getNumNormals() && bNormals && mesh.usingNormals() ){
		glEnableClientState(GL_NORMAL_ARRAY);
		glNormalPointer(GL_FLOAT, sizeof(ofVec3f), &mesh.getNormalsPointer()->x);
	}
	if(mesh.getNumColors() && mesh.usingColors() ){
		glEnableClientState(GL_COLOR_ARRAY);
		glColorPointer(4,GL_FLOAT, sizeof(ofFloatColor), &mesh.getColorsPointer()->r);
	}
    
	if(mesh.getNumTexCoords() && bTexCoords && mesh.usingTextures()){
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2, GL_FLOAT, sizeof(ofVec2f), &mesh.getTexCoordsPointer()->x);
	}
    
    if(aBeginIndex + aCount > mesh.getNumVertices()) {
//        cout << "ofxStrip :: draw : clamping index range" << endl;
        aCount = ofClamp( mesh.getNumVertices() - aBeginIndex, 0, mesh.getNumVertices() );
    }
    
    glDrawArrays(ofGetGLPrimitiveMode( mesh.getMode()), aBeginIndex, aCount );
    
	if(mesh.usingColors()) {
		glDisableClientState(GL_COLOR_ARRAY);
	}
	if(mesh.usingNormals() && bNormals){
		glDisableClientState(GL_NORMAL_ARRAY);
	}
	if(mesh.usingTextures() && bTexCoords){
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}
}

//--------------------------------------------------------------
void ofxStrip::setTexCoordScale( float tex_u_scale, float tex_v_scale ){
    tex_u = tex_u_scale;
    tex_v = tex_v_scale;
}

//--------------------------------------------------------------
void ofxStrip::setTexCoordRange( float aTexXStart, float aTexXEnd, float aTexYStart, float aTexYEnd ) {
    stex_u  = aTexXStart;
    tex_u   = aTexXEnd;
    stex_v  = aTexYStart;
    tex_v   = aTexYEnd;
}

//--------------------------------------------------------------
void ofxStrip::enableTexCoords(){
    bTexCoords = true;
}

//--------------------------------------------------------------
void ofxStrip::disableTexCoords(){
    bTexCoords = false;
}

//--------------------------------------------------------------
void ofxStrip::enableNormals(){
    bNormals = true;
}

//--------------------------------------------------------------
void ofxStrip::disableNormals(){
    bNormals = false;
}

//--------------------------------------------------------------
ofMesh ofxStrip::getMesh(){
    return mesh;
}

//--------------------------------------------------------------
ofMesh& ofxStrip::getMeshRef(){
    return mesh;
}




