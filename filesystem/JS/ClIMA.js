function ClIMA(firstClIMA,lastClIMA)
{
var div = document.getElementById("ClIMA");
div.style.width="700px";
div.className="CSSTab";
div.id="ClIMA";

var tbl = document.createElement('table');

var thead = document.createElement('thead');

// Build TOP TH		
	var th = document.createElement('th');

// Row 1
		th.textContent="#";
		th.rowSpan="2";
		th.width="30";
	var tr = document.createElement('tr');
	tr.appendChild(th);

		var th = document.createElement('th');
		th.textContent="custom Name of resource ClIMA.X";
		th.width="303";
	tr.appendChild(th);

		var th = document.createElement('th');
		th.textContent="Type";
		th.rowSpan="2";
		th.width="76";
	tr.appendChild(th);

		var th = document.createElement('th');
		th.textContent="Caps";
		th.rowSpan="2";
		th.width="76";
	tr.appendChild(th);

		var th = document.createElement('th');
		th.textContent="Temp";
		th.rowSpan="2";
		th.width="76";
	tr.appendChild(th);

		var th = document.createElement('th');
		th.textContent="Humi";
		th.rowSpan="2";
		th.width="77";
	tr.appendChild(th);
thead.appendChild(tr);

// Row 2

		var th = document.createElement('th');
		th.textContent="last resource activity Time Stamp";
	var tr = document.createElement('tr');
	tr.appendChild(th);
thead.appendChild(tr);


// Build TD life in Query!
var tbody = document.createElement('tbody');
tbody.id="tbClIMA";


// Build BOTTOM TH
var tfoot = document.createElement('tfoot');

// Row 1	
		var th = document.createElement('th');
		th.textContent="?";
		th.rowSpan="2";
	var tr = document.createElement('tr');
	tr.appendChild(th);

		var th = document.createElement('th');
		th.textContent="Name [a-zA-Z0-9_.]";
	tr.appendChild(th);

		var th = document.createElement('th');
		th.textContent="Type";
		th.rowSpan="2";
	tr.appendChild(th);

		var th = document.createElement('th');
		th.textContent="Caps";
		th.rowSpan="2";
	tr.appendChild(th);

		var th = document.createElement('th');
		th.textContent="Temp";
		th.rowSpan="2";
	tr.appendChild(th);

		var th = document.createElement('th');
		th.textContent="Humi";
		th.rowSpan="2";
	tr.appendChild(th);
tfoot.appendChild(tr);

// Row 2
		var th = document.createElement('th');
		th.textContent="TiSt [DD.MM.YYYY, HH:MM:SS]";
	var tr = document.createElement('tr');
	tr.appendChild(th);
tfoot.appendChild(tr);

// Row 3, info
		var th = document.createElement('th');
		th.id="NFOC";
		th.colSpan="6";
	var tr = document.createElement('tr');
	tr.appendChild(th);
tfoot.appendChild(tr);

// inits
div.firstClIMA=firstClIMA;
div.lastClIMA=lastClIMA;
div.timeStamp=-1;

// Finnish Table
tbl.appendChild(thead);
tbl.appendChild(tbody);
tbl.appendChild(tfoot);
div.appendChild(tbl);

PollClIMA();
}



function RptPollClIMA()
{
	var div = document.getElementById("ClIMA");
	if (div.timeStamp == "0")
		{
		setTimeout(PollClIMA, 5000);
		}
	else
		{
		PollClIMA();
		}
}



function PollClIMA() 
{
  var div = document.getElementById("ClIMA");

  RXxmlhttpClIMA=new XMLHttpRequest();
  RXxmlhttpClIMA.timeout = 40000;
  RXxmlhttpClIMA.ontimeout = function () {RXxmlhttpClIMA.abort();}
  RXxmlhttpClIMA.open("POST","/ClIMA.jso",true);

  if (div.timeStamp != -1)
	{
	RXxmlhttpClIMA.send("TIST="+encodeURIComponent(div.timeStamp)
		+"&fClIMA="+encodeURIComponent(div.firstClIMA)
		+"&lClIMA="+encodeURIComponent(div.lastClIMA));
	}
  else 
	{
	RXxmlhttpClIMA.send("fClIMA="+encodeURIComponent(div.firstClIMA)
		+"&lClIMA="+encodeURIComponent(div.lastClIMA));
	}

  RXxmlhttpClIMA.onreadystatechange=function()
	{
	if (RXxmlhttpClIMA.readyState==4 && RXxmlhttpClIMA.status>=200 && RXxmlhttpClIMA.status<300)
		{
		var data=JSON.parse(RXxmlhttpClIMA.responseText);

		var tbody = document.getElementById("tbClIMA");
		var j=1;

		for (var i=div.firstClIMA; i<=div.lastClIMA; i++)
			{
			if (data.STATUS.ClIMA[j-1]) if (data.STATUS.ClIMA[j-1].NO<i) j++;

			if (data.STATUS.ClIMA[j-1]) if (data.STATUS.ClIMA[j-1].NO==i)
				{
				// (re)construct row?
				var y = document.getElementById("aC"+i);
				if (!y) 
//------------------------------------------------- Table construction (if data) start -----------------------------------------------
{
					// 1. Row
					var td = document.createElement('td');
					td.id="NOC"+i;
					td.rowSpan="2";
					td.innerHTML=data.STATUS.ClIMA[j-1].NO;
						var tr = document.createElement('tr');
						tr.id="aC"+i;
						tr.appendChild(td);

					var input = document.createElement('input');
					input.style.textAlign="left";
					input.name="Name";
					input.id="NC"+i;
					input.type="text";
					input.size="31";
					input.maxLength="31";
					input.minLength="1";
					input.pattern="^[a-zA-Z0-9]+([a-zA-Z0-9._]+[a-zA-Z0-9])?$";
 					input.title="Level.Room.Device"; 
					input.slot=i;
					input.value=data.STATUS.ClIMA[j-1].Name;
					input.onblur=function(){this.hasFocus=false;};
					input.onfocus=function(){this.hasFocus=true;};
					input.onchange=function(){AJAXPostClIMA('NC'+this.slot);};
						var td = document.createElement('td');
						td.appendChild(input);
						td.height="40";
							tr.appendChild(td);

					var td = document.createElement('td');
					td.rowSpan="2";
					td.id="YC"+i;
						tr.appendChild(td);

					var td = document.createElement('td');
					td.id="CC"+i;
					td.rowSpan="2";
						tr.appendChild(td);

					var td = document.createElement('td');
					td.rowSpan="2";
					td.id="TC"+i;
					td.innerHTML=data.STATUS.ClIMA[j-1].TMP;
						tr.appendChild(td);

					var td = document.createElement('td');
					td.rowSpan="2";
					td.id="HC"+i;
					td.innerHTML=data.STATUS.ClIMA[j-1].HUM;
						tr.appendChild(td);

							// add or replace row
							var y = document.getElementById("aC"+i);
							if (y) tbody.replaceChild(tr,y);
							else tbody.appendChild(tr);

					// 2. Row

					var td = document.createElement('td');
					td.className="TS";
					td.id="TISTC"+i;
					td.height="40";
					var dt = new Date(data.STATUS.ClIMA[j-1].TiSt*1000);
					td.innerHTML=dt.toLocaleString('de-AT');
						var tr = document.createElement('tr');
						tr.id="bC"+i;
						tr.appendChild(td);

							// add or replace row
							var y = document.getElementById("bC"+i);
							if (y) tbody.replaceChild(tr,y);
							else tbody.appendChild(tr);
					}
//------------------------------------------------- Table construction (if data) end -----------------------------------------------
				else
//------------------------------------------------- Table data sync (if data) start -----------------------------------------------
					{
					var x=document.getElementById('NOC'+i);
					//if (!x.hasFocus) x.innerHTML=data.STATUS.ClIMA[j-1].NO;
					x.innerHTML=data.STATUS.ClIMA[j-1].NO;

					var x=document.getElementById('NC'+i);
					if (!x.hasFocus) x.value=data.STATUS.ClIMA[j-1].Name;

					var x = document.getElementById("YC"+i);
					x.innerHTML="X";

					var x = document.getElementById("CC"+i);
					x.innerHTML="X";

					var x = document.getElementById("TC"+i);
					x.innerHTML=data.STATUS.ClIMA[j-1].TMP;

					var x = document.getElementById("HC"+i);
					x.innerHTML=data.STATUS.ClIMA[j-1].HUM;

					var x=document.getElementById('TISTC'+i);
					var dt = new Date(data.STATUS.ClIMA[j-1].TiSt*1000);
					x.innerHTML=dt.toLocaleString('de-AT');
					}
//------------------------------------------------- Table data sync (if data) end -----------------------------------------------
				}



/*

				if (!data.STATUS.ClIMA[j-1].NO)
					{
				if (data.STATUS.ClIMA[j-1].NO==i)
else // delete unused rows, if any
					{
					// row 1
					var y = document.getElementById("aC"+i);
					if (y) tbody.removeChild(y);

					// row 2
					var y = document.getElementById("bC"+i);
					if (y) tbody.removeChild(y);
					}
*/
			//	}
			}

		div.timeStamp=(data.STATUS.TIME);

		var AkTIST = new Date();
		var DevTIST = new Date(data.STATUS.TIME*1000);

		var Info = document.getElementById("NFOC");
		Info.innerHTML="Local Time: " + 
			AkTIST.toLocaleTimeString('de-AT') +
			" - Data Validity: "+ DevTIST.toLocaleTimeString('de-AT') +
			" - Device Memory: " + data.STATUS.MEMORY + " Bytes";

		setTimeout(RptPollClIMA, 500);
		}
	}
}



function AJAXPostClIMA(formId)
{
	var elem = document.getElementById(formId);
	var params = elem.name + "=" + encodeURIComponent(elem.value);

	if (window.XMLHttpRequest)
		{
		// code for IE7+, Firefox, Chrome, Opera, Safari
		TXxmlhttpClIMA = new XMLHttpRequest();
		}
	else
		{ 
		// code for IE6, IE5
		TXxmlhttpClIMA=new ActiveXObject("Microsoft.XMLHTTP");
		}

	TXxmlhttpClIMA.open("POST","/"+elem.slot+"/ClIMA.cgi",true);
	TXxmlhttpClIMA.send(params);

	return TXxmlhttpClIMA.responseText;
}


