
//
//  RISOpenglText.h
//  LFClientEngine
//
//  Created by David Dvergsten on 1/25/17.
//  Copyright © 2017 Rapid Imaging Software Inc. All rights reserved.
//
#pragma once
#include <memory>
#include "SViewData.h"
#include <vector>
#include <QOpenGLTexture>
#include <QOpenGLShaderProgram>
#include <LFDefinitionsRA.h>
#include <QOpenGLBuffer>
#include "LoggableObject.h"
class RISOpenglText : public LoggableConfigurableObject

{
protected:
    virtual void SetUserSettingsReference(boost::shared_ptr<UserSettingsData> usettingsData);
private:
   // boost::shared_ptr<UserSettingsData> _usettingsdata;
    boost::shared_ptr<UserSettingsData> _usettingsdata;
    double GetTextWidth(std::string text, double symScale);
public:
    int Init(boost::shared_ptr<UserSettingsData> data, boost::shared_ptr<StatusLogger> logger, std::string classID);

   // QOpenGLShaderProgram* _program;
    QOpenGLBuffer indexBuf;
    QOpenGLBuffer arrayBuf;
    void RenderFont(std::string text, QMatrix4x4 modViewProjMatrix,
                    QMatrix4x4 projMatrix, QOpenGLShaderProgram* program);
    QOpenGLShaderProgram* _program;
    void setProgram(QOpenGLShaderProgram* program){_program = program;};
    double degreesToRadiansWithDegree(double degrees);
    RISOpenglText();
    int GetIndexValAsciiVal(int asciival);
    QOpenGLTexture* loadTextureWithFilename(std::string filename, std::string fileType);
    void LoadFontRow(int top, int bottom, int offset, const int * widthsRow, int size);
    void makeTextGridTextureCurrent();
    void loadFonts();
    void RenderFontXpos(double xpos, double ypos, double zpos,
                    std::string text, double scale, double rotation, bool tiltUp,
                    double shiftX, double shiftY, double shiftZ, bool centerX, double xrot, double yrot, double zrot, double frameWidth,
                    double frameHeight, boost::shared_ptr<SViewData> viewData, double camx,
                    double camy, double camz, QOpenGLShaderProgram* program, QMatrix4x4 projectionMatrix, double starePointX, double starePointY);

    void RenderFontXposStreetLabel(double xpos, double ypos, double zpos,
                    std::string text, double scale, double rotation, bool tiltUp,
                    double shiftX, double shiftY, double shiftZ, bool centerX, double xrot, double yrot, double zrot, double frameWidth,
                    double frameHeight, boost::shared_ptr<SViewData> viewData, double camx,
                    double camy, double camz, QOpenGLShaderProgram* program, QMatrix4x4 projectionMatrix);

    double CalculateStringLength(std::string text);
    double getCharWidth(char character);
    QOpenGLTexture * _letterGridTextureRA;
    //TODO: CREATE A BUFFER CONTAINING BOTH VERTICES AND TEX COORDS IE VertexDataLW
    VertexDataLW vertexData[2048];//todo: this can be smaller eventually
    //GLfloat         _positionVertices[2048];
    //GLfloat         _textureVertices[1024];
    double          _charWidths[256];
    std::vector<int> _letterBuffers;
    bool _fontLoaded;

};
