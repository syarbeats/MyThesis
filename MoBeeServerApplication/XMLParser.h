// XMLElement.h

#include <string>
#include <vector>
#include <map>
#include <iostream>

class XMLElement
{
 public:
  XMLElement();
  ~XMLElement();

  void setElementName(const std::string& inName);

  void setAttribute(const std::string& inAttributeName,
		    const std::string& inAttributeValue);

  void addSubElement(const XMLElement* inElement);

  // Setting a text node will override any nested elements
  void setTextNode(const std::string& inValue);

  friend std::ostream& operator<<(std::ostream& outStream,
			     const XMLElement& inElem);

 protected:
  void writeToStream(std::ostream& outStream, int inIndentLevel = 0) const;

  void indentStream(std::ostream& outStream, int inIndentLevel) const;

 private:
  std::string                            mElementName;
  std::map<std::string, std::string>     mAttributes;
  std::vector<const XMLElement*>         mSubElements;
  std::string                            mTextNode;
};
