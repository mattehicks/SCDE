function ADC(firstADC,lastADC)
{
var div = document.getElementById("ADC");
div.style.width="700px";
div.className="CSSTab";
div.id="ADC";

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
		th.textContent="custom Name of resource ADC.X";
		th.width="303";
	tr.appendChild(th);

		var th = document.createElement('th');
		th.textContent="AMIN";
		th.rowSpan="2";
		th.width="52";
	tr.appendChild(th);

		var th = document.createElement('th');
		th.textContent="AMAX";
		th.rowSpan="2";
		th.width="52";
	tr.appendChild(th);

		var th = document.createElement('th');
		th.textContent="OVSP";
		th.rowSpan="2";
		th.width="67";
	tr.appendChild(th);

		var th = document.createElement('th');
		th.textContent="VALUE";
		th.rowSpan="2";
		th.width="67";
	tr.appendChild(th);

		var th = document.createElement('th');
		th.textContent="Percent";
		th.rowSpan="2";
		th.width="67";
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
tbody.id="tbADC";


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
		th.textContent="AMIN";
		th.rowSpan="2";
	tr.appendChild(th);

		var th = document.createElement('th');
		th.textContent="AMAX";
		th.rowSpan="2";
	tr.appendChild(th);

		var th = document.createElement('th');
		th.textContent="OVSP";
		th.rowSpan="2";
	tr.appendChild(th);

		var th = document.createElement('th');
		th.textContent="VALUE";
		th.rowSpan="2";
	tr.appendChild(th);

		var th = document.createElement('th');
		th.textContent="Percent";
		th.rowSpan="2";
	tr.appendChild(th);

tfoot.appendChild(tr);

// Row 2
	var tr = document.createElement('tr');
		var th = document.createElement('th');
		th.textContent="TiSt [DD.MM.YYYY, HH:MM:SS]";
		tr.appendChild(th);

tfoot.appendChild(tr);

// Row 3, info
	var tr = document.createElement('tr');
		var th = document.createElement('th');
		th.id="NFOA";
		th.colSpan="7";
	tr.appendChild(th);
tfoot.appendChild(tr);


// inits
div.firstADC=firstADC;
div.lastADC=lastADC;
div.timeStamp=-1;


// Finnish Table
tbl.appendChild(thead);
tbl.appendChild(tbody);
tbl.appendChild(tfoot);
div.appendChild(tbl);

PollADC();
}



function RptPollADC()
{
	var div = document.getElementById("ADC");
	if (div.timeStamp == "0")
		{
		setTimeout(PollADC, 5000);
		}
	else
		{
		PollADC();
		}
}



function PollADC() 
{
  var div = document.getElementById("ADC");

  RXxmlhttpADC=new XMLHttpRequest();
  RXxmlhttpADC.timeout = 40000;
  RXxmlhttpADC.ontimeout = function () {RXxmlhttpADC.abort();}
  RXxmlhttpADC.open("POST","/ADC.jso",true);

  if (div.timeStamp != -1)
	{
	RXxmlhttpADC.send("TIST="+encodeURIComponent(div.timeStamp)
		+"&fADC="+encodeURIComponent(div.firstADC)
		+"&lADC="+encodeURIComponent(div.lastADC));
	}
  else 
	{
	RXxmlhttpADC.send("fADC="+encodeURIComponent(div.firstADC)
		+"&lADC="+encodeURIComponent(div.lastADC));
	}

  RXxmlhttpADC.onreadystatechange=function()
	{
	if (RXxmlhttpADC.readyState==4 && RXxmlhttpADC.status>=200 && RXxmlhttpADC.status<300)
		{
		var data=JSON.parse(RXxmlhttpADC.responseText);

		var tbody = document.getElementById("tbADC");
		var j=1;

		for (var i=div.firstADC; i<=div.lastADC; i++)
			{
			if (data.STATUS.ADC[j-1]) if (data.STATUS.ADC[j-1].NO<i) j++;

			if (data.STATUS.ADC[j-1]) if (data.STATUS.ADC[j-1].NO==i)
				{
				// (re)construct row?
				var y = document.getElementById("aA"+i);
				if (!y) 
//------------------------------------------------- Table construction (if data) start -----------------------------------------------
					{
					// 1. Row
					var td = document.createElement('td');
					td.id="NOA"+i;
					td.rowSpan="2";
					td.innerHTML=data.STATUS.ADC[j-1].NO;
						var tr = document.createElement('tr');
						tr.id="aA"+i;
						tr.appendChild(td);

					var input = document.createElement('input');
					input.style.textAlign="left";
					input.name="Name";
					input.id="NA"+i;
					input.type="text";
					input.size="31";
					input.maxLength="31";
					input.minLength="1";
					input.pattern="^[a-zA-Z0-9]+([a-zA-Z0-9._]+[a-zA-Z0-9])?$";
 					input.title="Level.Room.Device"; 
					input.slot=i;
					input.value=data.STATUS.ADC[j-1].Name;
					input.onblur=function(){this.hasFocus=false;};
					input.onfocus=function(){this.hasFocus=true;};
					input.onchange=function(){AJAXPostADC('NA'+this.slot);};
						var td = document.createElement('td');
						td.appendChild(input);
						td.height="40";
							tr.appendChild(td);

					var input = document.createElement('input');
					input.name="AMIN";
					input.id="AMINA"+i;
					input.type="text";
					input.size="4";
					input.maxLength="4";
					input.slot=i;
					//input.value=data.STATUS.ADC[j-1].NAME;
					input.onblur=function(){this.hasFocus=false;};
					input.onfocus=function(){this.hasFocus=true;};
					input.onchange=function(){AJAXPostADC('AMINA'+this.slot);};
						var td = document.createElement('td');
						td.rowSpan="2";
						td.appendChild(input);
							tr.appendChild(td);

					var input = document.createElement('input');
					input.name="AMAX";
					input.id="AMAXA"+i;
					input.type="text";
					input.size="4";
					input.maxLength="4";
					//input.value=data.STATUS.ADC[j-1].NAME;
					input.onblur=function(){this.hasFocus=false;};
					input.onfocus=function(){this.hasFocus=true;};
					input.onchange=function(){AJAXPostADC('AMAXA'+this.slot);};
						var td = document.createElement('td');
						td.rowSpan="2";
						td.appendChild(input);
							tr.appendChild(td);

					var input = document.createElement('input');
					input.name="OVSP";
					input.id="OVSPA"+i;
					input.type="text";
					input.size="4";
					input.maxLength="4";
					//input.value=data.STATUS.ADC[j-1].NAME;
					input.onblur=function(){this.hasFocus=false;};
					input.onfocus=function(){this.hasFocus=true;};
					input.onchange=function(){AJAXPostADC('OVSPA'+this.slot);};
						var td = document.createElement('td');
						td.rowSpan="2";
						td.appendChild(input);
							tr.appendChild(td);

					var td = document.createElement('td');
					td.rowSpan="2";
					td.id="RA"+i;
					td.innerHTML=data.STATUS.ADC[j-1].RAW;
						tr.appendChild(td);

					var td = document.createElement('td');
					td.rowSpan="2";
					td.id="RX"+i;
					//td.innerHTML=data.STATUS.ADC[j-1].NO;
						tr.appendChild(td);

							// add or replace row
							var y = document.getElementById("aA"+i);
							if (y) tbody.replaceChild(tr,y);
							else tbody.appendChild(tr);

					// 2. Row
					var td = document.createElement('td');
					td.className="TS";
					td.id="TISTA"+i;
					td.height="40";
					var dt = new Date(data.STATUS.ADC[j-1].TiSt*1000);
					td.innerHTML=dt.toLocaleString('de-AT');
						var tr = document.createElement('tr');
						tr.id="bA"+i;
						tr.appendChild(td);

					
							// add or replace row
							var y = document.getElementById("bA"+i);
							if (y) tbody.replaceChild(tr,y);
							else tbody.appendChild(tr);
					}
//------------------------------------------------- Table construction (if data) end -----------------------------------------------
				else
//------------------------------------------------- Table data sync (if data) start -----------------------------------------------
					{
					var x=document.getElementById('NOA'+i);
					//if (!x.hasFocus) x.innerHTML=data.STATUS.ADC[j-1].NO;
					x.innerHTML=data.STATUS.ADC[j-1].NO;

					var x=document.getElementById('NA'+i);
					if (!x.hasFocus) x.value=data.STATUS.ADC[j-1].Name;

					var x=document.getElementById('RA'+i);
					x.innerHTML=data.STATUS.ADC[j-1].RAW;

					var x=document.getElementById('TISTA'+i);
					var dt = new Date(data.STATUS.ADC[j-1].TIST*1000);
					x.innerHTML=dt.toLocaleString('de-AT');
					}
//------------------------------------------------- Table data sync (if data) end -----------------------------------------------
				}



/*

				if (!data.STATUS.ADC[j-1].NO)
					{
				if (data.STATUS.ADC[j-1].NO==i)
else // delete unused rows, if any
					{
					// row 1
					var y = document.getElementById("aA"+i);
					if (y) tbody.removeChild(y);

					// row 2
					var y = document.getElementById("bA"+i);
					if (y) tbody.removeChild(y);
					}
*/
			//	}
			}

		div.timeStamp=(data.STATUS.TIME);

		var AkTIST = new Date();
		var DevTIST = new Date(data.STATUS.TIME*1000);

		var Info = document.getElementById("NFOA");
		Info.innerHTML="Local Time: " + 
			AkTIST.toLocaleTimeString('de-AT') +
			" - Data Validity: "+ DevTIST.toLocaleTimeString('de-AT') +
			" - Device Memory: " + data.STATUS.MEMORY + " Bytes";

		setTimeout(RptPollADC, 500);
		}
	}
}



function AJAXPostADC(formId)
{
	var elem = document.getElementById(formId);
	var params = elem.name + "=" + encodeURIComponent(elem.value);

	if (window.XMLHttpRequest)
		{
		// code for IE7+, Firefox, Chrome, Opera, Safari
		TXxmlhttpADC = new XMLHttpRequest();
		}
	else
		{ 
		// code for IE6, IE5
		TXxmlhttpADC=new ActiveXObject("Microsoft.XMLHTTP");
		}

	TXxmlhttpADC.open("POST","/"+elem.slot+"/ADC.cgi",true);
	TXxmlhttpADC.send(params);

	return TXxmlhttpADC.responseText;
}


