#include "dlnastoragefolder.h"

DlnaStorageFolder::DlnaStorageFolder(QString host, int port, QObject *parent):
    DlnaResource(parent),
    host(host),
    port(port)
{
}

/*
* Returns XML (DIDL) representation of the DLNA node. It gives a
* complete representation of the item, with as many tags as available.
*
* Reference: http://www.upnp.org/specs/av/UPnP-av-ContentDirectory-v1-Service.pdf
*/
QDomElement DlnaStorageFolder::getXmlContentDirectory(QDomDocument *xml, QStringList properties) {
    if (!xml)
        return QDomElement();

    QDomElement xml_obj = xml->createElement("container");

    updateXmlContentDirectory(xml, &xml_obj, properties);

    if (properties.contains("*") || properties.contains("@childCount")) {
        xml_obj.setAttribute("childCount", QString("%1").arg(getChildrenSize()));
    }

    return xml_obj;
}
