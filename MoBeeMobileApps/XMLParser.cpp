/****************************************************************************
**
** Trolltech hereby grants a license to use the Qt/Eclipse Integration
** plug-in (the software contained herein), in binary form, solely for the
** purpose of creating code to be used with Trolltech's Qt software.
**
** Qt Designer is licensed under the terms of the GNU General Public
** License versions 2.0 and 3.0 ("GPL License"). Trolltech offers users the
** right to use certain no GPL licensed software under the terms of its GPL
** Exception version 1.2 (http://trolltech.com/products/qt/gplexception).
**
** THIS SOFTWARE IS PROVIDED BY TROLLTECH AND ITS CONTRIBUTORS (IF ANY) "AS
** IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
** TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
** PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER
** OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
** EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
** PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
** PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
** LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
** NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
** SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** Since we now have the GPL exception I think that the "special exception
** is no longer needed. The license text proposed above (other than the
** special exception portion of it) is the BSD license and we have added
** the BSD license as a permissible license under the exception.
**
****************************************************************************/

#include "XMLParser.h"
#include <QMap>
#include <QFile>
#include <QMessageBox>
#include <QXmlStreamWriter>
#include <QMapIterator>
#include <QDateTime>


XMLParser::XMLParser()   
{
	
}

void XMLParser::insertXML(QMap<QString,QString> strAttribute, QString strAtributeName, QString strAttributeValue)
{
	strAttribute.insertMulti(strAtributeName,strAttributeValue);
}

void XMLParser::CreateXMLFile(QString strImei, QString filename)
{
	QFile file(filename);
	//if (!file.open(QFile::ReadOnly | QFile::Text))
	if (!file.open(QIODevice::WriteOnly))
	{
	QMessageBox::warning(0, "Read only", "The file is in read only mode");
	}	
	else
	{
		QXmlStreamWriter* xmlWriter = new QXmlStreamWriter();
		xmlWriter->setDevice(&file);
		xmlWriter->writeStartDocument();
		xmlWriter->writeStartElement("client");
		QMapIterator<QString, QString> i(imsi);
		QMapIterator<QString, QString> j(cellid);
		QMapIterator<QString, QString> k(latitude);
		QMapIterator<QString, QString> l(longitude);
		QMapIterator<QString, QString> m(datetime);
		QMapIterator<QString, QString> n(OperatorName);
		QMapIterator<QString, QString> o(NetworkName);
	    
		while (i.hasNext() && j.hasNext() && k.hasNext()&& l.hasNext()&& m.hasNext()&& n.hasNext()&& o.hasNext()) 
	    {
			i.next(); j.next();	k.next(); l.next(); m.next(); n.next(); o.next();
			xmlWriter->writeStartElement("client");
			xmlWriter->writeAttribute(i.key(), i.value());
			xmlWriter->writeAttribute(j.key(), j.value());
			xmlWriter->writeAttribute(k.key(), k.value());
			xmlWriter->writeAttribute(l.key(), l.value());
			xmlWriter->writeAttribute(m.key(), m.value());
			xmlWriter->writeAttribute(n.key(), n.value());
			xmlWriter->writeAttribute(o.key(), o.value());
			xmlWriter->writeCharacters (strImei);
			xmlWriter->writeEndElement();
	      
	    } 
	    
		xmlWriter->writeEndElement();
		xmlWriter->writeEndDocument();
	}
}

XMLParser::~XMLParser()
{

}

void XMLParser::createMenus()
{
   
}

