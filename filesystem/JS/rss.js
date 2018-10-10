//http://www.xml.com/pub/a/2006/09/13/rss-and-ajax-a-simple-news-reader.html?page=5



function getRSS(url)
{
	
    /*A*/
    if (window.ActiveXObject) //IE
        xhr = new ActiveXObject("Microsoft.XMLHTTP");
    else if (window.XMLHttpRequest) //other
        xhr = new XMLHttpRequest();
    else
        alert("your browser does not support AJAX");

    /*B*/
   // xhr.open("GET","ecsuha.de/news/rss/2",true);
 //   xhr.open("GET","http://ecsuha.de/news/rss/2");
//    xhr.open("GET",document.rssform.rssurl.value,true);

    xhr.open("GET",url,true);

    /*C*/
  //  xhr.setRequestHeader("Cache-Control", "no-cache");
 //   xhr.setRequestHeader("Pragma", "no-cache");

    /*D*/
    xhr.onreadystatechange = function() {
        if (xhr.readyState == 4)
        {
            if (xhr.status == 200)
            {
                /*F*/
                if (xhr.responseText != null)
                    processRSS(xhr.responseXML);
                else
                {
                    alert("Failed to receive RSS file from the server - file not found.");
                    return false;
                }
            }
            else
                alert("Error code " + xhr.status + " received: " + xhr.statusText);
        }
    }

    /*E*/
    xhr.send(null);
}



function processRSS(rssxml)
{
    RSS = new RSS2Channel(rssxml);
    showRSS(RSS);
}



function RSS2Channel(rssxml)
{
 // required string properties
 this.link;
 this.title;
 this.description;

 // optional string properties
 this.language;
 this.copyright;
 this.managingEditor;
 this.webMaster;
 this.pubDate;
 this.lastBuildDate;
 this.generator;
 this.docs;
 this.ttl;
 this.rating;

 // optional object properties
// this.category;
 this.image;

 // new array of RSS2Item objects
 this.items = new Array();

 // fix on ch0
 var chanElement = rssxml.getElementsByTagName("channel")[0];
 // get number of item elements
 var itemElements = rssxml.getElementsByTagName("item");

 // read item elements
 for (var i=0; i<itemElements.length; i++)
	{
	Item = new RSS2Item(itemElements[i]);
	this.items.push(Item);
	}

 // ???
 var properties = new Array("title", "link", "description", "language", "copyright", "managingEditor", "webMaster", "pubDate", "lastBuildDate", "generator", "docs", "ttl", "rating");
 var tmpElement = null;
 for (var i=0; i<properties.length; i++)
	{
	tmpElement = chanElement.getElementsByTagName(properties[i])[0];
	if (tmpElement!= null)
	eval("this."+properties[i]+"=tmpElement.childNodes[0].nodeValue");
	}

 // category object
// this.category = new RSS2Category(chanElement.getElementsByTagName("category")[0]);
 // image object
// this.image = new RSS2Image(chanElement.getElementsByTagName("image")[0]);
}



/*
function RSS2Category(catElement)
{
    if (catElement == null) {
        this.domain = null;
        this.value = null;
    } else {
        this.domain = catElement.getAttribute("domain");
        this.value = catElement.childNodes[0].nodeValue;
    }
}
*/



function RSS2Item(itemxml)
{
    /*A*/
    /*required properties (strings)*/
    this.title;
    this.link;
    this.description;

    /*optional properties (strings)*/
    this.author;
    this.comments;
    this.pubDate;

    /*optional properties (objects)*/
    this.category;
    this.enclosure;
    this.guid;
    this.source;

    /*B*/
    var properties = new Array("title", "link", "description", "author", "comments", "pubDate");
    var tmpElement = null;
    for (var i=0; i<properties.length; i++)
    {
        tmpElement = itemxml.getElementsByTagName(properties[i])[0];
        if (tmpElement != null)
            eval("this."+properties[i]+"=tmpElement.childNodes[0].nodeValue");
    }

    /*C*/
//    this.category = new RSS2Category(itemxml.getElementsByTagName("category")[0]);
 //   this.enclosure = new RSS2Enclosure(itemxml.getElementsByTagName("enclosure")[0]);
   this.guid = new RSS2Guid(itemxml.getElementsByTagName("guid")[0]);
//    this.source = new RSS2Source(itemxml.getElementsByTagName("source")[0]);
}



function RSS2Enclosure(encElement)
{
    if (encElement == null) {
        this.url = null;
        this.length = null;
        this.type = null;
    } else {
        this.url = encElement.getAttribute("url");
        this.length = encElement.getAttribute("length");
        this.type = encElement.getAttribute("type");
    }
}

function RSS2Guid(guidElement)
{
    if (guidElement == null) {
        this.isPermaLink = null;
        this.value = null;
    } else {
        this.isPermaLink = guidElement.getAttribute("isPermaLink");
        this.value = guidElement.childNodes[0].nodeValue;
    }
}

function RSS2Source(souElement)
{
    if (souElement == null) {
        this.url = null;
        this.value = null;
    } else {
        this.url = souElement.getAttribute("url");
        this.value = souElement.childNodes[0].nodeValue;
    }
}




function showRSS(RSS)
{
 document.getElementById('chan_head').innerHTML = "<b>"+eval("RSS.title")+"</b>";
/*+" "+eval("RSS.description")*/

 document.getElementById("chan_items").innerHTML = "";
 for (var i=0; i<RSS.items.length; i++)
	{
	item_html = "<hr>";
	item_html += (RSS.items[i].title == null) ? ""		: "<p><b>"+RSS.items[i].title+"</b></p>";
	item_html += (RSS.items[i].description == null) ? ""	: "<p>"+RSS.items[i].description+"</p>";
	item_html += (RSS.items[i].link == null) ? ""		: "<p><a href="+RSS.items[i].link+">weiterlesen ...</a></p>";
	document.getElementById("chan_items").innerHTML += item_html;
	}
 return true;
}
