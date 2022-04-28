
#include<string>

class Texture
{
private:
	unsigned int m_RendererId;
	std::string m_imagePath;
	unsigned char* m_localBuffer;
	int width, height, m_BPP;
public:
	Texture(const std::string& filePath);
	~Texture();

	void Bind(unsigned int slot = 0) const;
	void Unbind();
};
