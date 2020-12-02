#include "opengltext.h"


void opengltext::SetUserSettings()
{
    //todo: fill in with any settings needed from settings.ini file
}

opengltext::opengltext(boost::shared_ptr<openglfont> font, double textScale, boost::shared_ptr<UserSettingsData> usettingsData, boost::shared_ptr<StatusLogger> logger):
    _textScale(textScale),
    _numOfTextChars(0),
_totalAdvance(0)
{
    LoggableConfigurableObject::Init(usettingsData, logger, "opengltext");
    _openglFont = font;
}

//#if defined(__APPLE__)
void opengltext::setText(std::string text)
{
    _numOfTextChars = text.size();
    //_arrayBuf.create();
    //_arrayBuf.bind();
    //_textScale = 100.0;
    _text = text;
    int textSize = text.size();
    _lineVertices.clear();
    //use openglfont to grab spacing between text etc, and create an arraybuff
    //where each item has texture coords along with vertex coords.
    //int totalAdvance = 0;
    for(int i = 0 ; i < text.size() ; i ++)
    {
        double lowerNormalizedx = 0.0;
        double lowerNormalizedy = 0.0;
        double upperNormalizedx = 0.0;
        double upperNormalizedy = 0.0;
        int width = 0;
        int height = 0;
        int advance = 0;
        int originX = 0;
        int originY = 0;
        int charx = 0;
        int chary = 0;
        unsigned char cc = text[i];
        _openglFont->getLetterTextureDimensions(cc, lowerNormalizedx, lowerNormalizedy, upperNormalizedx, upperNormalizedy, width, height, advance, originX, originY, charx, chary);
        _totalAdvance += advance;
    }
    
    // Center the text at the origin
    int x = -(static_cast<double>(_totalAdvance) / 2.0);
    int fontSize = _openglFont->GetFontSize();
    int y = static_cast<double>(fontSize) / 2.0;
    int fontWidth = _openglFont->GetFontWidth();
    int fontHeight = _openglFont->GetFontHeight();
    for(int i = 0 ; i < text.size() ; i ++)
    {
        int index = i * 6;
        unsigned char cc = text[i];
        
        double lowerNormalizedx = 0.0;
        double lowerNormalizedy = 0.0;
        double upperNormalizedx = 0.0;
        double upperNormalizedy = 0.0;
        
        int width = 0;
        int height = 0;
        int advance = 0;
        int originX = 0;
        int originY = 0;
        int charx = 0;
        int chary = 0;
        _openglFont->getLetterTextureDimensions(cc, lowerNormalizedx, lowerNormalizedy, upperNormalizedx, upperNormalizedy, width, height, advance, originX, originY, charx, chary);
        
        //        // Measure how wide the text is
        //        for (var i = 0; i < text.length; i++) {
        //          totalAdvance += font.characters[text[i]].advance;
        //        }
        
        //        // Center the text at the origin
        //        var x = -totalAdvance / 2;
        //        var y = font.size / 2;
        //        var vertices = [];
        
        //        // Add a quad for each character
        //        for (var i = 0; i < text.length; i++) {
        //          var c = font.characters[text[i]];
        
        //          // p0 --- p1
        //          // | \     |
        //          // |   \   |
        //          // |     \ |
        //          // p2 --- p3
        
        int x0 = x - originX;
        int y0 = -( y - originY);
        float s0 = static_cast<float>(charx) / static_cast<float>(fontWidth);
        float t0 = static_cast<float>(fontHeight - chary) / static_cast<float>(fontHeight);
        
        int x1 = x - originX + width;
        int y1 = -(y - originY);
        float s1 = static_cast<float>(charx + width) / static_cast<float>(fontWidth);
        float t1 = static_cast<float>(fontHeight - chary) / static_cast<float>(fontHeight);
        
        int x2 = x - originX;
        int y2 = -(y - originY + height);
        float s2 = static_cast<float>(charx) / static_cast<float>(fontWidth);
        float t2 = static_cast<float>(fontHeight - chary - height) / static_cast<float>(fontHeight);
        
        int x3 = x - originX + width;
        int y3 = -(y - originY + height);
        float s3 = static_cast<float>(charx + width) / static_cast<float>(fontWidth);
        float t3 = static_cast<float>(fontHeight - chary - height) / static_cast<float>(fontHeight);
        
        t0 = 1.0 - t0;
        t1 = 1.0 - t1;
        t2 = 1.0 - t2;
        t3 = 1.0 - t3;
        float zposition = 2.0;
        _lineVertices.push_back((float)x2); //fill this in with the values given by font for w/h of letter
        _lineVertices.push_back((float) y2);
        _lineVertices.push_back((float) zposition);
        //we're just using a pixel since this is a solid color
        _lineVertices.push_back((float)s2);
        _lineVertices.push_back((float)t2);

        _lineVertices.push_back((float) x3); //fill this in with the values given by font for w/h of letter
        _lineVertices.push_back((float) y3);
        _lineVertices.push_back((float) zposition);
        //we're just using a pixel since this is a solid color
        _lineVertices.push_back( (float)s3);
        _lineVertices.push_back( (float)t3);

        _lineVertices.push_back((float) x0); //fill this in with the values given by font for w/h of letter
        _lineVertices.push_back((float) y0);
        _lineVertices.push_back((float) zposition);
        //we're just using a pixel since this is a solid color
        _lineVertices.push_back((float)s0);
        _lineVertices.push_back( (float)t0);



        _lineVertices.push_back((float) x3); //fill this in with the values given by font for w/h of letter
        _lineVertices.push_back( (float) y3);
        _lineVertices.push_back( (float) zposition);
        //we're just using a pixel since this is a solid color
        _lineVertices.push_back( (float)s3);
        _lineVertices.push_back( (float)t3);


        _lineVertices.push_back((float) x1); //fill this in with the values given by font for w/h of letter
        _lineVertices.push_back( (float) y1);
        _lineVertices.push_back( (float) zposition);
        //we're just using a pixel since this is a solid color
        _lineVertices.push_back( (float)s1);
        _lineVertices.push_back( (float)t1);

        _lineVertices.push_back( (float) x0); //fill this in with the values given by font for w/h of letter
        _lineVertices.push_back( (float) y0);
        _lineVertices.push_back((float) zposition);
        //we're just using a pixel since this is a solid color
        _lineVertices.push_back( (float)s0);
        _lineVertices.push_back((float)t0);
        
        x += advance ;
    }
    
    glGenBuffers(1, &_instanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, _instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, _lineVertices.size() * sizeof(float), &_lineVertices[0], GL_STATIC_DRAW);
    //glBindBuffer(GL_ARRAY_BUFFER, 0);
    //_arrayBuf.create();
    //_arrayBuf.bind();
    
    //_arrayBuf.allocate(_lineVertices.get(), 6  * _numOfTextChars * sizeof(VertexDataLW));
    
}
//#else
//void opengltext::setText(std::string text)
//{
//    _numOfTextChars = text.size();
//
//    _arrayBuf.create();
//    _arrayBuf.bind();
//
//    //_textScale = 100.0;
//    _text = text;
//    int textSize = text.size();
//
//
//    _lineVertices.reset(new VertexDataLW[6 * textSize]);
//
//    //use openglfont to grab spacing between text etc, and create an arraybuff
//    //where each item has texture coords along with vertex coords.
//    //int totalAdvance = 0;
//    for(int i = 0 ; i < text.size() ; i ++)
//    {
//        double lowerNormalizedx = 0.0;
//        double lowerNormalizedy = 0.0;
//        double upperNormalizedx = 0.0;
//        double upperNormalizedy = 0.0;
//        int width = 0;
//        int height = 0;
//        int advance = 0;
//        int originX = 0;
//        int originY = 0;
//        int charx = 0;
//        int chary = 0;
//        unsigned char cc = text[i];
//        _openglFont->getLetterTextureDimensions(cc, lowerNormalizedx, lowerNormalizedy, upperNormalizedx, upperNormalizedy, width, height, advance, originX, originY, charx, chary);
//        _totalAdvance += advance;
//    }
//
//    // Center the text at the origin
//    int x = -(static_cast<double>(totalAdvance) / 2.0);
//    int fontSize = _openglFont->GetFontSize();
//    int y = static_cast<double>(fontSize) / 2.0;
//    int fontWidth = _openglFont->GetFontWidth();
//    int fontHeight = _openglFont->GetFontHeight();
//    for(int i = 0 ; i < text.size() ; i ++)
//    {
//        int index = i * 6;
//        unsigned char cc = text[i];
//
//        double lowerNormalizedx = 0.0;
//        double lowerNormalizedy = 0.0;
//        double upperNormalizedx = 0.0;
//        double upperNormalizedy = 0.0;
//
//        int width = 0;
//        int height = 0;
//        int advance = 0;
//        int originX = 0;
//        int originY = 0;
//        int charx = 0;
//        int chary = 0;
//        _openglFont->getLetterTextureDimensions(cc, lowerNormalizedx, lowerNormalizedy, upperNormalizedx, upperNormalizedy, width, height, advance, originX, originY, charx, chary);
//
////        // Measure how wide the text is
////        for (var i = 0; i < text.length; i++) {
////          totalAdvance += font.characters[text[i]].advance;
////        }
//
////        // Center the text at the origin
////        var x = -totalAdvance / 2;
////        var y = font.size / 2;
////        var vertices = [];
//
////        // Add a quad for each character
////        for (var i = 0; i < text.length; i++) {
////          var c = font.characters[text[i]];
//
////          // p0 --- p1
////          // | \     |
////          // |   \   |
////          // |     \ |
////          // p2 --- p3
//
//          int x0 = x - originX;
//          int y0 = -( y - originY);
//          float s0 = static_cast<float>(charx) / static_cast<float>(fontWidth);
//          float t0 = static_cast<float>(fontHeight - chary) / static_cast<float>(fontHeight);
//
//          int x1 = x - originX + width;
//          int y1 = -(y - originY);
//          float s1 = static_cast<float>(charx + width) / static_cast<float>(fontWidth);
//          float t1 = static_cast<float>(fontHeight - chary) / static_cast<float>(fontHeight);
//
//          int x2 = x - originX;
//          int y2 = -(y - originY + height);
//          float s2 = static_cast<float>(charx) / static_cast<float>(fontWidth);
//          float t2 = static_cast<float>(fontHeight - chary - height) / static_cast<float>(fontHeight);
//
//          int x3 = x - originX + width;
//          int y3 = -(y - originY + height);
//          float s3 = static_cast<float>(charx + width) / static_cast<float>(fontWidth);
//          float t3 = static_cast<float>(fontHeight - chary - height) / static_cast<float>(fontHeight);
//
//          _lineVertices[index + 0].position.setX(((float) x2)); //fill this in with the values given by font for w/h of letter
//          _lineVertices[index + 0].position.setY(((float) y2));
//          _lineVertices[index + 0].position.setZ(((float) -4.0));
//          //we're just using a pixel since this is a solid color
//          _lineVertices[index + 0].texCoord.setX(s2);
//          _lineVertices[index + 0].texCoord.setY(t2);
//
//          _lineVertices[index + 1].position.setX(((float) x3)); //fill this in with the values given by font for w/h of letter
//          _lineVertices[index + 1].position.setY(((float) y3));
//          _lineVertices[index + 1].position.setZ(((float) -4.0));
//          //we're just using a pixel since this is a solid color
//          _lineVertices[index + 1].texCoord.setX(s3);
//          _lineVertices[index + 1].texCoord.setY(t3);
//
//          _lineVertices[index + 2].position.setX(((float) x0)); //fill this in with the values given by font for w/h of letter
//          _lineVertices[index + 2].position.setY(((float) y0));
//          _lineVertices[index + 2].position.setZ(((float) -4.0));
//          //we're just using a pixel since this is a solid color
//          _lineVertices[index + 2].texCoord.setX(s0);
//          _lineVertices[index + 2].texCoord.setY(t0);
//
//
//
//          _lineVertices[index + 3].position.setX(((float) x3)); //fill this in with the values given by font for w/h of letter
//          _lineVertices[index + 3].position.setY(((float) y3));
//          _lineVertices[index + 3].position.setZ(((float) -4.0));
//          //we're just using a pixel since this is a solid color
//          _lineVertices[index + 3].texCoord.setX(s3);
//          _lineVertices[index + 3].texCoord.setY(t3);
//
//
//          _lineVertices[index + 4].position.setX(((float) x1)); //fill this in with the values given by font for w/h of letter
//          _lineVertices[index + 4].position.setY(((float) y1));
//          _lineVertices[index + 4].position.setZ(((float) -4.0));
//          //we're just using a pixel since this is a solid color
//          _lineVertices[index + 4].texCoord.setX(s1);
//          _lineVertices[index + 4].texCoord.setY(t1);
//
//          _lineVertices[index + 5].position.setX(((float) x0)); //fill this in with the values given by font for w/h of letter
//          _lineVertices[index + 5].position.setY(((float) y0));
//          _lineVertices[index + 5].position.setZ(((float) -4.0));
//          //we're just using a pixel since this is a solid color
//          _lineVertices[index + 5].texCoord.setX(s0);
//          _lineVertices[index + 5].texCoord.setY(t0);
//
//          x += advance ;
//    }
//    _arrayBuf.create();
//    _arrayBuf.bind();
//
//    _arrayBuf.allocate(_lineVertices.get(), 6  * _numOfTextChars * sizeof(VertexDataLW));
//
//}
//#endif

#if defined(__APPLE__)
void opengltext::render(GLuint program)
{
    //set arraybuff current
    //call openglfont with bindfonttexture
    glBindBuffer(GL_ARRAY_BUFFER, _instanceVBO);
    //_arrayBuf.bind();
    
    int offset = 0;
    
    //_apositionLocation = glGetAttribLocation ( _risShaderProgram, "a_position" );
    //_atextureLocation = glGetAttribLocation ( _risShaderProgram, "a_texCoord" );
    
    int vertexCoordLoc = glGetAttribLocation(program,"a_position");
    int textureCoordLoc = glGetAttribLocation(program,"a_texCoord");
    
    
    //glBindBuffer(GL_ARRAY_BUFFER, _instanceVBO);
    //GLuint vertexLoc = glGetAttribLocation(program, "InVertex");
    
    glVertexAttribPointer(vertexCoordLoc, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(0 * sizeof(GLfloat)));
    glEnableVertexAttribArray(vertexCoordLoc);
    
    
    glVertexAttribPointer(textureCoordLoc, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(textureCoordLoc);
    
    //int vertexLocation = program->attributeLocation("a_position");
    //program->enableAttributeArray(vertexLocation);
    //program->setAttributeBuffer(vertexLocation, GL_FLOAT, offset, 3, sizeof(VertexDataLW));
    
    // Offset for texture coordinate
    //offset += sizeof(QVector3D);
    
    // Tell OpenGL programmable pipeline how to locate vertex texture coordinate data
    //int texcoordLocation = program->attributeLocation("a_texcoord");
    //program->enableAttributeArray(texcoordLocation);
    //program->setAttributeBuffer(texcoordLocation, GL_FLOAT, offset, 2, sizeof(VertexDataLW));
    
    _openglFont->bindFont();//ie bind font.png texture
    
    glDrawArrays(GL_TRIANGLES, 0, 6 * _numOfTextChars);
    
    //int offset = 0;
}

void opengltext::setJustification(int justification)
{
    
}

void opengltext::setBasePoint(int x, int y)
{
    
}

#else
void opengltext::render(QOpenGLShaderProgram* program)
{
    //set arraybuff current
    //call openglfont with bindfonttexture
    _arrayBuf.bind();

    int offset = 0;
    int vertexLocation = program->attributeLocation("a_position");
    program->enableAttributeArray(vertexLocation);
    program->setAttributeBuffer(vertexLocation, GL_FLOAT, offset, 3, sizeof(VertexDataLW));

    // Offset for texture coordinate
    offset += sizeof(QVector3D);

    // Tell OpenGL programmable pipeline how to locate vertex texture coordinate data
    int texcoordLocation = program->attributeLocation("a_texcoord");
    program->enableAttributeArray(texcoordLocation);
    program->setAttributeBuffer(texcoordLocation, GL_FLOAT, offset, 2, sizeof(VertexDataLW));

    _openglFont->bindFont();//ie bind font.png texture

    glDrawArrays(GL_TRIANGLES, 0, 6 * _numOfTextChars);

    //int offset = 0;
}

void opengltext::setJustification(int justification)
{

}

void opengltext::setBasePoint(int x, int y)
{

}
#endif
