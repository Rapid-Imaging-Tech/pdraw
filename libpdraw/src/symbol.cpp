#include "symbol.h"
#include <iostream>
#include "RotationTK.h"
#include "utilities.h"
#include "RIMatrixMath.hpp"
#if defined(__APPLE__)
#define ICONSIZE 30.0
extern GLuint vao ;
extern GLuint gbl_u_SamplerLocation;
extern GLuint gbl_u_mvpLocation ;
extern GLuint gbl_u_colorLocation ;
#else
#define ICONSIZE 15.0
#endif
#define FLAGPOLEOFFSET 50.0
using namespace std;

void symbol::SetUserSettings(){
    
    if(!(_userSettingsData->_intSettings.find("TEXTSCALE") == _userSettingsData->_intSettings.end())){
        int iTextScale = _userSettingsData->_intSettings["TEXTSCALE"];
        _textScale = static_cast<double>(iTextScale) / 1000.0;
    }
    else
        this->_textScale = 10.0;
}

//TODO: WHY NOT DO THIS ON STARTUP ONCE FOR EACH SYMBOL TYPE, THEN JUST SEND A BOOST SHARED_PTR
//  TO EACH VECTORTILE CLASS AND JUST USE THAT
void symbol::InitOpenGLEntities()//boost::shared_ptr<UserSettingsData> usettingsData)
{
    float ytop = 1.0;
    float ybottom = 0.0;
#if defined(__APPLE__)
	bool flipicony = true;
	float zposition = 2.0;
#else
    bool flipicony = false;
	float zposition = -4.0;
#endif    
    if(flipicony)
    {
        float temp = ytop;
        ytop = ybottom;
        ybottom = temp;
    }
        
    //WriteToStatusLog("createing symbol", false, true);
    _lineVertices.clear();
    _lineVertices.push_back(((float) -ICONSIZE));
    _lineVertices.push_back(((float) (ICONSIZE * 2) ));
    _lineVertices.push_back(((float) zposition));
    //we're just using a pixel since this is a solid color
    _lineVertices.push_back((float)0.0);
    _lineVertices.push_back((float)ytop);
    
    
    //TS: set these up as 2 triangles
    _lineVertices.push_back(((float) -ICONSIZE));
    _lineVertices.push_back(((float) 0.0 ));
    _lineVertices.push_back(((float) zposition));
    //we're just using a pixel since this is a solid color
    _lineVertices.push_back((float)0.0);
    _lineVertices.push_back((float)ybottom);
    
    
    _lineVertices.push_back(((float) ICONSIZE));
    _lineVertices.push_back(((float) (ICONSIZE * 2) ));
    _lineVertices.push_back(((float) zposition));
    //we're just using a pixel since this is a solid color
    _lineVertices.push_back((float)1.0);
    _lineVertices.push_back((float)ytop);
    
    
    _lineVertices.push_back(((float) ICONSIZE));
    _lineVertices.push_back(((float) 0.0 ));
    _lineVertices.push_back(((float) zposition));
    //we're just using a pixel since this is a solid color
    _lineVertices.push_back((float)1.0);
    _lineVertices.push_back((float)ybottom);
    
    
    //draw flagpole
    _lineVertices.push_back(((float) 0.0));
    _lineVertices.push_back(((float) 0.0 ));
    _lineVertices.push_back(((float) zposition));
    //we're just using a pixel since this is a solid color
    _lineVertices.push_back((float)0.5);
    _lineVertices.push_back((float)0.5);
    
    _lineVertices.push_back(((float) 0.0));
    _lineVertices.push_back(((float) FLAGPOLEOFFSET));
    _lineVertices.push_back(((float) zposition));
    //we're just using a pixel since this is a solid color
    _lineVertices.push_back((float)0.6);
    _lineVertices.push_back((float)0.6);

#if defined(__APPLE__)
	glGenBuffers(1, &_instanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, _instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, _lineVertices.size() * sizeof(float), &_lineVertices[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
#else
    //Tegra VideoPlayer stuff
    _arrayBuf.create();
    _arrayBuf.bind();
    _arrayBuf.allocate(&_lineVertices[0], 6 * sizeof(float) * 5);
#endif
    _openglText->setText(_symbolText);
    _entitiesInitialized = true;
}

void symbol::UpdateDistToVehicle(double vehLat, double vehLon)
{
    _distToVehicle = distLatLonQuick(vehLat, _lat, vehLon, _lon);
}

symbol::~symbol()
{
#if !defined(__APPLE__)
    if(_arrayBuf.isCreated())
        _arrayBuf.destroy();
#endif
}
#if defined(__APPLE__)
symbol::symbol(double lat, double lon, double xcartesian, double ycartesian, double zcartesian, GLuint texture,
               boost::shared_ptr<openglfont> font, std::string text, boost::shared_ptr<UserSettingsData> usettingsData, boost::shared_ptr<StatusLogger> logger,
               GLuint whiteTexture, boost::shared_ptr<iconInfo> icInfo):
#else
symbol::symbol(double lat, double lon, double xcartesian, double ycartesian, double zcartesian, boost::shared_ptr<QOpenGLTexture> texture,
               boost::shared_ptr<openglfont> font, std::string text, boost::shared_ptr<UserSettingsData> usettingsData, boost::shared_ptr<StatusLogger> logger,
               boost::shared_ptr<QOpenGLTexture> whiteTexture, boost::shared_ptr<iconInfo> icInfo):
#endif
    _textScale(10.0),
    _entitiesInitialized(false),
    _distToVehicle(0.00),
    _rotationTK(usettingsData, logger),
    //_customTexture(0x00),
    _kmlPathDetails(new KmlPathDetails),
    _visible(true)


{
    LoggableConfigurableObject::Init(usettingsData, logger, "Symbol");
    _iconInfo = icInfo;

    _openglFont = font;
    _openglText.reset(new opengltext(_openglFont, _textScale, usettingsData, logger));

    _symbolText = text;
    _symbolTexture = texture;

    _symbolTextureWhite = whiteTexture;
    _lat = lat;
    _lon = lon;
    _cartesianBillboardPositions[1].reset(new symbolCartesianPoint(xcartesian, ycartesian, zcartesian));
    //_cartesianBase.reset(new symbolCartesianPoint(xcartesian, ycartesian, zcartesian));
//    _xcartesianBase = xcartesian;
//    _ycartesianBase = ycartesian;
//    _zcartesianBase = zcartesian;

};
//bool symbol::IsCurrentBillboardLevel(int currentBillboardLevel){
//    bool isCurrentLevel = true;
//    if(currentBillboardLevel == 1){
//        bool levelMismatch = false;
//        switch(this->_iconInfo->_pmType){
//            case SL_STREETLABELX2:
//                levelMismatch = true;
//                break;
//            case SL_STREETLABELX4:
//                levelMismatch = true;
//                break;
//            case SL_STREETLABELX8:
//                levelMismatch = true;
//                break;
//            case SL_STREETLABELX16:
//                levelMismatch = true;
//                break;
//            default: //not a street billboard, do nothing
//                break;
//        }
//        if(levelMismatch)
//            isCurrentLevel = false;//ie if this is a level2 street billboard, and the current level the user selected is level1, then don't show this label
//    }
//    if(currentBillboardLevel == 2){
//        bool levelMismatch = false;
//        switch(this->_iconInfo->_pmType){
//            case SL_STREETLABELX1:
//                levelMismatch = true;
//                break;
//            case SL_STREETLABELX4:
//                levelMismatch = true;
//                break;
//            case SL_STREETLABELX8:
//                levelMismatch = true;
//                break;
//            case SL_STREETLABELX16:
//                levelMismatch = true;
//                break;
//            default: //not a street billboard, do nothing
//                break;
//        }
//        if(levelMismatch)
//            isCurrentLevel = false;//ie if this is a level2 street billboard, and the current level the user selected is level1, then don't show this label
//    }
//    if(currentBillboardLevel == 4){
//        bool levelMismatch = false;
//        switch(this->_iconInfo->_pmType){
//            case SL_STREETLABELX1:
//                levelMismatch = true;
//                break;
//            case SL_STREETLABELX2:
//                levelMismatch = true;
//                break;
//            case SL_STREETLABELX8:
//                levelMismatch = true;
//                break;
//            case SL_STREETLABELX16:
//                levelMismatch = true;
//                break;
//            default: //not a street billboard, do nothing
//                break;
//        }
//        if(levelMismatch)
//            isCurrentLevel = false;//ie if this is a level2 street billboard, and the current level the user selected is level1, then don't show this label
//    }
//    if(currentBillboardLevel == 8){
//        bool levelMismatch = false;
//        switch(this->_iconInfo->_pmType){
//            case SL_STREETLABELX1:
//                levelMismatch = true;
//                break;
//            case SL_STREETLABELX2:
//                levelMismatch = true;
//                break;
//            case SL_STREETLABELX4:
//                levelMismatch = true;
//                break;
//            case SL_STREETLABELX16:
//                levelMismatch = true;
//                break;
//            default: //not a street billboard, do nothing
//                break;
//        }
//        if(levelMismatch)
//            isCurrentLevel = false;//ie if this is a level2 street billboard, and the current level the user selected is level1, then don't show this label
//    }
//    if(currentBillboardLevel == 16){
//        bool levelMismatch = false;
//        switch(this->_iconInfo->_pmType){
//            case SL_STREETLABELX1:
//                levelMismatch = true;
//                break;
//            case SL_STREETLABELX2:
//                levelMismatch = true;
//                break;
//            case SL_STREETLABELX4:
//                levelMismatch = true;
//                break;
//            case SL_STREETLABELX8:
//                levelMismatch = true;
//                break;
//            default: //not a street billboard, do nothing
//                break;
//        }
//        if(levelMismatch)
//            isCurrentLevel = false;//ie if this is a level2 street billboard, and the current level the user selected is level1, then don't show this label
//    }
//    return isCurrentLevel;
//}

#include <math.h>
#if defined(__APPLE__)
void symbol::render(boost::shared_ptr<SViewData> viewData, double windowWidth, double windowHeight, GLuint program,  double vehx, double vehy, double vehz, RIMatrix4x4 projection, std::vector<double>& alreadyDrawnIcons, bool billboardStyle, int currentLabelDensityLevel)
#else
void symbol::render(boost::shared_ptr<SViewData> viewData, double windowWidth, double windowHeight, QOpenGLShaderProgram* program,  double vehx, double vehy, double vehz, QMatrix4x4 projection)
#endif
{
    //BBOARD: SET THESE ACCORDINGLY DEPENDING ON WHIHC BILLBOARD LEVEL WE DESIRE
    //assume its not an x2 4 8 16 billboard
    double xcartesian = _cartesianBillboardPositions[1]->_x;// _cartesianBase->_x;// _xcartesianBase;
    double ycartesian = _cartesianBillboardPositions[1]->_y;//_cartesianBase->_y;//_ycartesianBase;
    double zcartesian = _cartesianBillboardPositions[1]->_z;//_cartesianBase->_z;//_zcartesianBase;
    
    int startIndex = 1;
    int startNextLevelIndex = 2;
    boost::shared_ptr<symbolCartesianPoint> cartesianPoint;
    if(this->_iconInfo->_pmType == SL_STREETLABELX1 && billboardStyle == false){
        return;
    }
    if(this->_iconInfo->_pmType == SL_STREETLABELX1)//this only should take effect when we're looking at a billboarded street label, else use start/end 1/2
    {
        //then we need to loop through based on currentLabelDensityLevel
        //  ie if x2 then loop through 2 and 3, update xcartesian/y/z with the coordinates for 2 3
        switch (currentLabelDensityLevel)
        {
            case 1:
                startIndex = 1;
                startNextLevelIndex = 2;
                break;
            case 2:
                startIndex = 2;
                startNextLevelIndex = 4;
                break;
            case 4:
                startIndex = 4;
                startNextLevelIndex = 8;
                break;
            case 8:
                startIndex = 8;
                startNextLevelIndex = 16;
                break;
            case 16:
                startIndex = 16;
                startNextLevelIndex = 32;//currently there is no [32] so don't try access that
                break;
            
            default://default this is just a plain symbol not street label or a 1x street label
                startIndex = 1;
                startNextLevelIndex = 2;
                break;
        }
    }
    for(int iii = startIndex; iii < startNextLevelIndex ; iii++)
    {
        if (NULL == _cartesianBillboardPositions[iii]) {
            xcartesian = 0;
            ycartesian = 0;
            zcartesian = 0;
        } else {
            xcartesian = _cartesianBillboardPositions[iii]->_x;// _cartesianBase->_x;// _xcartesianBase;
            ycartesian = _cartesianBillboardPositions[iii]->_y;//_cartesianBase->_y;//_ycartesianBase;
            zcartesian = _cartesianBillboardPositions[iii]->_z;//_cartesianBase->_z;//_zcartesianBase;
        }
    
        bool billboardStreetLabel = false;
    
        //attempt to declutter all the billboarded street labels
        //BBOARD: THE TYPE WILL NOW ONLY BE X1, AND WE CHECK THE LEVEL WE ARE AT, AND DO A FOR LOOP HERE FOR X2 X8  ETC
        //  CHANGING THE XYZ EACH TIME SO WE CAN INCREASE OUR STREETLABEL/BILLBOARD DENSITY
        if(this->_iconInfo->_pmType == SL_STREETLABELX1 || this->_iconInfo->_pmType == SL_STREETLABELX2|| this->_iconInfo->_pmType == SL_STREETLABELX4
           || this->_iconInfo->_pmType == SL_STREETLABELX8 || this->_iconInfo->_pmType == SL_STREETLABELX16)
        {
            //DENSITY2: GET RID OF THIS
            if(billboardStyle == false){ //&& pitch < -70 then forget this one
                return;//we're drawing these flat due to users choice, these will get rendered in renderPerspectiveentities
            }
            //then don't draw an icon, don't draw flag pole, lower text to street level
            billboardStreetLabel = true;
            
            //don't render street labels close to the horizon to eliminate clutter
            double deltax = xcartesian - vehx ;
            double deltay = ycartesian - vehy;//vehy - _ycartesian;
            double deltaz = vehz - zcartesian;
            double lengthStarePoint = sqrt(deltax*deltax + deltay*deltay + deltaz*deltaz);
            double radianPitch = asin(deltaz / lengthStarePoint);
            double degreePitch = 57.2958 * radianPitch;
            double pitch = -1 * degreePitch;//usually denoted as negative because below horizon
            if(pitch > -10.0)
            {
                //lets try declutter the stlabels on the horizon by comparing to labels already drawn
            
                //calc azimuth to streetlabel.  check list "alreadyDrawnIcons" to see if a stlabel was already drawn within 5 degrees.
                //if yes, then return, if no, then add this azimuth to the list and continue on to render.
                double radAzimuth = atan(deltax / deltay);
                double degAzimuth = 57.2958 * radAzimuth;
                double degHeading = degAzimuth;
                if(deltax < 0)
                    degHeading += 180.0;//we're in the western half
                bool foundLabelTooClose = false;
                for(int i = 0 ; i < alreadyDrawnIcons.size() ; i++)
                {
                    double val = alreadyDrawnIcons[i];
                    double deltaDegreeToVeh = abs(val - degHeading);
                    if(deltaDegreeToVeh < 10.0)
                    {
                        //we're too close to this stlabel dont draw it
                        foundLabelTooClose = true;
                        break;//we don't want to return from the function now, we want to
                    }
                }
                if(foundLabelTooClose)
                    continue;//we don't want to skip the rest of the billboard labels
                //if we made it this far we didn't find a label too close
                //continue on, and mark this down for other stlabels to check against
                alreadyDrawnIcons.push_back(degHeading);
            }
            //else if(pitch < -10 && pitch > -70){
            // draw the billboarded label with no filtering
            //else if(pitch<-70 && billboardstyle == false)
            //  bypass this one, it will be drawn in openglautoscaletext.
            //DENSITY2:
            //if billboardstyle == false, lets print out the horizon labels as usual, with the filtering method above, and print out anything with a pitch of-10 to -70 as a billboard without filtering
            //  and skip anthing from -70 to -90 and print those out as flat street labels
        }
        if(!_visible)
            return ;
        if(!_entitiesInitialized)
            return;
        //take p/h/r use the new rotationtk getpixel4geo routine.  At this point we should be in ortho space
        //so just draw to x,y for this symbol
        
        
        //calculate pitch from veh to this symbol.  if above 45 degrees(make configurable), then check with list of other symbols already
        //rendered and make sure this symbol isn't within 10 degrees (from veh) from other symbols.  if it is, then return, if not then
        //render and add to the global list for other symbols to check.
        
        //if below 45 degrees then always render
        
        double px = xcartesian - vehx;
        double py = ycartesian - vehy;
        double pz = zcartesian - vehz;
        
        double vp = 0.00;
        double vh = 0.00;
        double vr = 0.00;
        
        double cp = viewData->dCameraPitch;
        double ch = viewData->dCameraHeading;
        double cr = viewData->dCameraRoll;
        
        double hfov = viewData->dFovHorizontalAngle;
        double vfov = viewData->dfov;//viewData->dFovVerticalAngle;
        double nx = windowWidth;
        double ny = windowHeight;
        
        double sx = 0.00;
        double sy = 0.00;
        double distance = 0.00;
        double camcart[3],cart[3];
        
        camcart[0] = vehx;
        camcart[1] = vehy;
        camcart[2] = vehz;
        cart[0] = xcartesian;
        cart[1] = ycartesian;
        cart[2] = zcartesian;
        
        RIRotationTK riRotTK(windowWidth, windowHeight);
        riRotTK.SetCamera(camcart,-cp,ch,cr);
        riRotTK.SetFOVs(hfov, vfov);
        riRotTK.ResizeWindow(nx, ny);
        riRotTK.LoadMatrices();
    #if defined(__APPLE__)
        _rotationTK.SetCamera(camcart,-cp,ch,cr);//sets viewM in rotationtk
        _rotationTK.SetFOVs(hfov,vfov);//
        _rotationTK.ResizeWindow(nx,ny);
        _rotationTK.LoadMatrices();
        //QVector4D sc = _rotationTK.GetPixels4Cart(cart);
        RIVector4 sc = riRotTK.GetPixels4Cart(cart);
        //sx = sc[0];
        //sy = sc[1];
        float fsx, fsy, fsz, fsw;
        
        sc.GetValues(fsx, fsy, fsz, fsw);
        sx = fsx;//windowWidth-50.0;// / 2.0;
        sy = fsy;//windowHeight-50.0;// / 2.0;
        if(sx == -1 && sy == -1)
            continue;//these are behind the camera and should not draw
        //for now lets assume 100, 100 as screen coordinates unti pixel4geo is implemented

        

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBindBuffer(GL_ARRAY_BUFFER, _instanceVBO);
        
        int vertexCoordLoc = glGetAttribLocation ( program, "a_position" );
        
        // Offset for texture coordinate
        
        // Tell OpenGL programmable pipeline how to locate vertex texture coordinate data
        int textureCoordLoc = glGetAttribLocation ( program, "a_texCoord" );
        //program->enableAttributeArray(texcoordLocation);
        //program->setAttributeBuffer(texcoordLocation, GL_FLOAT, offset, 2, sizeof(VertexDataLW));
        // Bind the base map
        
        glActiveTexture ( GL_TEXTURE0 );
        glBindTexture(GL_TEXTURE_2D, _symbolTexture);

        
        // Set the base map sampler to texture unit to 0
        glUniform1i ( gbl_u_SamplerLocation, 0 );
        
        
        glBindBuffer(GL_ARRAY_BUFFER, _instanceVBO);

        
        glVertexAttribPointer(vertexCoordLoc, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(0 * sizeof(GLfloat)));
        glEnableVertexAttribArray(vertexCoordLoc);
        
        
        glVertexAttribPointer(textureCoordLoc, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(textureCoordLoc);
        
        double dgdCamPitch = cp + 90.0;
        double scale = dgdCamPitch / 90.0;
        
        double scaledFlagPoleHeight = scale * FLAGPOLEOFFSET;
        
        
        //DRAW ICON
        //vmath::mat4 matrix = vmath::translate((float) sx, (float) (sy + scaledFlagPoleHeight), (float) 0.0);
        RIMatrix4x4 rimatrix;
        rimatrix.SetToIdentity();
        float iconScale = 1.0;
        double textScale = 1.0;
        double textOffset = ICONSIZE * 2 + 10 * textScale + 10;
        if(!billboardStreetLabel)
        {//draw as regular poi with icon
            if(GetIconType() == SHIELD_HIGHWAY || GetIconType() == SHIELD_INTERSTATE ||GetIconType() == SHIELD_WHITERECT)
            {
                scaledFlagPoleHeight = 0.00;
            }
            
            rimatrix = rimatrix.PerformTranslate((float) sx, (float) (sy + scaledFlagPoleHeight), (float) 0.0);
            rimatrix = rimatrix.PerformScale(iconScale, iconScale, 1.0);
            
            ////////////////
            int textScaleOffset = 0;
            double shieldScale = 1.0;
            if(GetIconType() == SHIELD_HIGHWAY)
            {
                int textWidth = _openglText->GetTotalAdvance();
                int numChars = _openglText->GetNumChars();
                double xscale = 1.0;
                if(textWidth + 20 > ICONSIZE*2*shieldScale)
                {
                   //need to scale it up in the x direction
                   xscale = (textWidth + 20) / (ICONSIZE*2*shieldScale);
                }
                rimatrix = rimatrix.PerformScale( xscale, 1.0, 1.0);

                 //matrix.scale(shieldScale, shieldScale, 1.0);
            }
            else if(GetIconType() == SHIELD_INTERSTATE)
            {
                int textWidth = _openglText->GetTotalAdvance();
                int numChars = _openglText->GetNumChars();
                double xscale = 1.0;
                if(textWidth > ICONSIZE*2*shieldScale)
                {
                   //need to scale it up in the x direction
                   xscale = textWidth / (ICONSIZE*2*shieldScale);
                }

                rimatrix = rimatrix.PerformScale(shieldScale * xscale, shieldScale, 1.0);
                //iconYOffset = -10.0;
                textScaleOffset = 10.0;

            }
            else if(GetIconType() == SHIELD_WHITERECT)
            {
                int textWidth = _openglText->GetTotalAdvance();
                int numChars = _openglText->GetNumChars();
                 double xscale = 1.0;
                 int textMargin = 100;
                //if(((textWidth + textMargin) * 0.5) > ICONSIZE*2*shieldScale)
                {
                    //need to scale it up in the x direction
                    xscale = ((textWidth + textMargin) * 1.0) / (ICONSIZE*2*shieldScale);
                   // xscale = 1.0;
                }
                rimatrix = rimatrix.PerformTranslate(0.0 ,-10.0,  0.0);
                rimatrix = rimatrix.PerformScale(shieldScale * xscale,shieldScale, 1.0);
                 textScaleOffset = 10.0;
                //matrix.scale(1.7, 1.0, 1.0);

            }
            
            //////////////////////////
            
            RIMatrix4x4 rimvp_matrix = projection * rimatrix;
            glUniformMatrix4fv(gbl_u_mvpLocation, 1, GL_FALSE, rimvp_matrix.get());
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

            //////////DRAW FLAG POLE TO ICON
            //set white texture as current
            glActiveTexture ( GL_TEXTURE0 );
            glBindTexture(GL_TEXTURE_2D, _symbolTextureWhite);
            //_symbolTextureWhite->bind();
            //translate back down to ground level to start the flag pole at
            rimatrix = rimatrix.PerformTranslate((float)0.0, (float)(-1.0* scaledFlagPoleHeight), (float)0.0);
            glLineWidth(1.0);
            rimatrix = rimatrix.PerformScale(1.0, (float)scale, 1.0);
            
            
            rimvp_matrix = projection * rimatrix;
            
            
            
            
            glUniformMatrix4fv(gbl_u_mvpLocation, 1, GL_FALSE, rimvp_matrix.get());
            //double textOffset = ICONSIZE * 2 + 10;
            if(GetIconType() != SHIELD_HIGHWAY && GetIconType() != SHIELD_INTERSTATE&& GetIconType() != SHIELD_WHITERECT)
            {
               glDrawArrays(GL_LINE_STRIP, 4, 2);//draw the flagpole
            }
            else if(GetIconType() == SHIELD_HIGHWAY)
                textOffset = ICONSIZE + 3.0;
            else if(GetIconType() == SHIELD_WHITERECT)
                textOffset = ICONSIZE + -10.0 + textScaleOffset;
            else
                textOffset = ICONSIZE + 0.0 + textScaleOffset;
            
            //glDrawArrays(GL_LINE_STRIP, 4, 2);Ï
            //////////end DRAW FLAG POLE TO ICON
        }
        
        /////////start draw text
        //move up an additional ICONSIZE/height + another 10 se we're above the symbol
        
        rimatrix.SetToIdentity();
        if(!billboardStreetLabel){
            rimatrix = rimatrix.PerformTranslate((float)sx, (float)(sy + textOffset + scaledFlagPoleHeight), (float)0.0);
        }
        else{
            rimatrix = rimatrix.PerformTranslate((float)sx, (float)(sy + 10), (float)0.0);
        }
        rimatrix = rimatrix.PerformScale((float)textScale, (float)textScale, (float)1.0);
        RIMatrix4x4 rimvp_matrix = projection * rimatrix;
        glUniformMatrix4fv(gbl_u_mvpLocation, 1, GL_FALSE, rimvp_matrix.get());
        
        _openglText->render(program);
        ///////////end draw text
    #else
    //    //_rotationTK.LoadMatrices();
    //
    //     //RIVector4 ri_sc = riRotTK.GetPixels4Cart(cart);
    //    RIVector4 sc = riRotTK.GetPixels4Cart(cart);
    //    float x, y, z, w;
    //    sc.GetValues(x, y, z, w);
    //    sx = x;
    //    sy = y;
    //
    //    if(sx == -1 && sy == -1)
    //        return;//these are behind the camera and should not draw
    //    //for now lets assume 100, 100 as screen coordinates unti pixel4geo is implemented
    //    _arrayBuf.bind();
    //
    //    int offset = 0;
    //    int vertexLocation = program->attributeLocation("a_position");
    //    program->enableAttributeArray(vertexLocation);
    //    program->setAttributeBuffer(vertexLocation, GL_FLOAT, offset, 3, sizeof(VertexDataLW));
    //
    //    // Offset for texture coordinate
    //    offset += sizeof(QVector3D);
    //
    //    // Tell OpenGL programmable pipeline how to locate vertex texture coordinate data
    //    int texcoordLocation = program->attributeLocation("a_texcoord");
    //    program->enableAttributeArray(texcoordLocation);
    //    program->setAttributeBuffer(texcoordLocation, GL_FLOAT, offset, 2, sizeof(VertexDataLW));
    //
    //    _symbolTexture->bind();
    //    _symbolTexture->setMagnificationFilter(QOpenGLTexture::Linear);
    //    _symbolTexture->setMinificationFilter(QOpenGLTexture::Linear);
    //    _arrayBuf.bind();
    //
    //    double dgdCamPitch = cp + 90.0;
    //    double scale = dgdCamPitch / 90.0;
    //
    //    double scaledFlagPoleHeight = scale * FLAGPOLEOFFSET;
    //
    //    QMatrix4x4 matrix;
    //    matrix.setToIdentity();
    //    matrix.translate((GLdouble) sx ,(GLdouble) (sy + scaledFlagPoleHeight),(GLdouble) 0.0);
    //    program->setUniformValue("mvp_matrix", projection * matrix);
    //     glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    //
    //     _symbolTextureWhite->bind();
    //
    //     matrix.translate((GLdouble) 0.0 ,(GLdouble) (-1.0 * (scaledFlagPoleHeight)),(GLdouble) 0.0);
    //
    //     glLineWidth(1.0);
    //     matrix.scale(1.0, scale, 1.0);
    //     program->setUniformValue("mvp_matrix", projection * matrix);
    //     glDrawArrays(GL_LINE_STRIP, 4, 2);
    //     /////////start draw text
    //      //move up an additional ICONSIZE/height + another 10 se we're above the symbol
    //      double textOffset = ICONSIZE * 2 + 10;
    //      matrix.setToIdentity();
    //      matrix.translate((GLdouble)sx, (GLdouble) (sy + textOffset + scaledFlagPoleHeight), (GLdouble)0.0);//move up in the y direction for text
    //
    //      matrix.scale(0.5);
    //      program->setUniformValue("mvp_matrix", projection * matrix);
    //      _openglText->render(program);
    //      ///////////end draw text
#endif
    }
}
