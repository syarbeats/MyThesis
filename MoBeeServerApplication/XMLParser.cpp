#include "XMLParser.h"

using namespace std;

XMLElement::XMLElement() : mElementName("unnamed"),  mTextNode("")
{
}

XMLElement::~XMLElement()
{
}

void XMLElement::setElementName(const string& inName)
{
  mElementName = inName;
}

void XMLElement::setAttribute(const string& inAttributeName, 
			      const string& inAttributeValue)
{
  // set the key/value pair, replacing the existing one if it exists
  mAttributes[inAttributeName] = inAttributeValue;
}

void XMLElement::addSubElement(const XMLElement* inElement)
{
  // add the new element to the vector of sub-elements
  mSubElements.push_back(inElement);
}

void XMLElement::setTextNode(const string& inValue)
{
  mTextNode = inValue;
}

ostream& operator<<(ostream& outStream, const XMLElement& inElem)
{
  inElem.writeToStream(outStream);
  return (outStream);
}


void XMLElement::writeToStream(ostream& outStream, int inIndentLevel) const
{
  indentStream(outStream, inIndentLevel);
  outStream << "<" << mElementName;  // open the start tag
  
  // output any attributes
  for (map<string, string>::const_iterator it = mAttributes.begin();
       it != mAttributes.end(); ++it) {
    outStream << " " << it->first << "=\"" << it->second << "\"";
  }

  // close the start tag
  outStream << ">";

  if (mTextNode != "") {
    // if there's a text node, output it
    outStream << mTextNode;
  } else {
    outStream << endl;
    // call writeToStream at inIndentLevel+1 for any sub-elements
    for (vector<const XMLElement*>::const_iterator it = mSubElements.begin();
	 it != mSubElements.end(); ++it) {
      (*it)->writeToStream(outStream, inIndentLevel + 1);
    }
    indentStream(outStream, inIndentLevel);
  }

  // write the close tag
  outStream << "</" << mElementName << ">" << endl;
}

void XMLElement::indentStream(ostream& outStream, int inIndentLevel) const
{
  for (int i = 0; i < inIndentLevel; i++) {
    outStream << "\t";
  }
}


/*
int main()
{
  XMLElement dialogueElement;
  dialogueElement.setElementName("dialogue");
  
  XMLElement sentenceElement1;
  sentenceElement1.setElementName("sentence");
  sentenceElement1.setAttribute("speaker", "Marni");
  sentenceElement1.setTextNode("Let's go get some ice cream.");

  XMLElement sentenceElement2;
  sentenceElement2.setElementName("sentence");
  sentenceElement2.setAttribute("speaker", "Scott");
  sentenceElement2.setTextNode("After I'm done writing this C++ book.");

  // Add the sentence elements as sub-elements of the dialogue element
  dialogueElement.addSubElement(&sentenceElement1);
  dialogueElement.addSubElement(&sentenceElement2);

  // output the dialogue element to stdout
  cout << dialogueElement;
}
*/
