
#include <iostream>
#include <string>
class GeneralText
{
public:
	GeneralText();
	~GeneralText();
	void DrawGeneralText(std::string text, int x, int y);
	bool Init();
private:
	unsigned int CreateBitmapFont(char *fontName, int fontSize);
	unsigned int m_fontListBase;
	void RenderFont(float xPos, float yPos, float zPos, unsigned int base, 
		const char *str, double scale,bool outline, bool textOnly);

};