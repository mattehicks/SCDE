function SwITCH(show_switch)
{
var div = document.getElementById("SwITCH");
div.style.width="700px";
div.className="CSSTab";
div.id="SwITCH";

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
		th.textContent="Custom Name of Resource SwITCH.X";
		th.width="303";
	tr.appendChild(th);

		var th = document.createElement('th');
		th.textContent="Val";
		th.rowSpan="2";
		th.width="46";
	tr.appendChild(th);

		var th = document.createElement('th');
		th.textContent="Min";
		th.rowSpan="2";
		th.width="46";
	tr.appendChild(th);

		var th = document.createElement('th');
		th.textContent="Value";
		th.rowSpan="2";
		th.width="100";
	tr.appendChild(th);

		var th = document.createElement('th');
		th.textContent="Max";
		th.rowSpan="2";
		th.width="59";
	tr.appendChild(th);

		var th = document.createElement('th');
		th.textContent="Dim";
		th.rowSpan="2";
		th.width="46";
	tr.appendChild(th);
thead.appendChild(tr);

// Row 2
		var th = document.createElement('th');
		th.textContent="Last resource activity Time Stamp";
	var tr = document.createElement('tr');
	tr.appendChild(th);
thead.appendChild(tr);


// Build TD life in Query!
var tbody = document.createElement('tbody');
tbody.id="tbSwITCH";


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
		th.textContent="val [0-9]";
		th.rowSpan="2";
	tr.appendChild(th);

		var th = document.createElement('th');
		th.textContent="min [ ]";
		th.rowSpan="2";
	tr.appendChild(th);

		var th = document.createElement('th');
		th.textContent="val [0-9]";
		th.rowSpan="2";
	tr.appendChild(th);

		var th = document.createElement('th');
		th.textContent="max [ ]";
		th.rowSpan="2";
	tr.appendChild(th);

		var th = document.createElement('th');
		th.textContent="dim [0-9]";
		th.rowSpan="2";
	tr.appendChild(th);
tfoot.appendChild(tr);

// Row 2
		var th = document.createElement('th');
		th.textContent="TiSt [dd.mm.yyyy, hh:mm:ss]";
	var tr = document.createElement('tr');
	tr.appendChild(th);
tfoot.appendChild(tr);

// Row 3, info
		var th = document.createElement('th');
		th.id="NFOS";
		th.colSpan="7";
	var tr = document.createElement('tr');
	tr.appendChild(th);
tfoot.appendChild(tr);

// inits
div.show_switch=show_switch;
div.time_stamp=-1;

// Finnish Table
tbl.appendChild(thead);
tbl.appendChild(tbody);
tbl.appendChild(tfoot);
div.appendChild(tbl);

Poll_SwITCH();
}



function RptPoll_SwITCH()
{

	var div = document.getElementById("SwITCH");

	if (div.time_stamp == "0") {
		setTimeout(Poll_SwITCH, 5000);
	}

	else {
		Poll_SwITCH();
	}
}



function Poll_SwITCH() 
{
  var div = document.getElementById("SwITCH");

  RXxmlhttpSwITCH=new XMLHttpRequest();
  RXxmlhttpSwITCH.timeout = 40000;
  RXxmlhttpSwITCH.ontimeout = function () {RXxmlhttpSwITCH.abort();}
  RXxmlhttpSwITCH.open("POST","/"+div.show_switch+".jso",true);

  if (div.time_stamp != -1) {

	RXxmlhttpSwITCH.send("TIST="+encodeURIComponent(div.time_stamp));
  }

  else {

	RXxmlhttpSwITCH.send();
  }

  RXxmlhttpSwITCH.onreadystatechange=function() {

	if (RXxmlhttpSwITCH.readyState==4 && RXxmlhttpSwITCH.status>=200 && RXxmlhttpSwITCH.status<300) {

		var data=JSON.parse(RXxmlhttpSwITCH.responseText);

		var tbody = document.getElementById("tbSwITCH");
		var j=1;

		for (var i=1; i<=1; i++) {//(var i=div.firstSwITCH; i<=div.lastSwITCH; i++) {

			if (data.STATUS.SwITCH[j-1]) if (data.STATUS.SwITCH[j-1].NO<i) j++;

			if (data.STATUS.SwITCH[j-1]) if (data.STATUS.SwITCH[j-1].NO==i) {

				// (re)construct row?
				var y = document.getElementById("aS"+i);

				if (!y) 
//------------------------------------------------- Table construction (if data) start -----------------------------------------------
					{
// 1. Row
					var td = document.createElement('td');
					td.id="NOS"+i;
					td.rowSpan="2";
					td.innerHTML=j-1;//data.STATUS.SwITCH[j-1].NO;
						var tr = document.createElement('tr');
						tr.id="aS"+i;
						tr.appendChild(td);

					var input = document.createElement('input');
					input.name="Name";
					input.id="NS"+i;
					input.slotname=data.STATUS.SwITCH[j-1].Name;
					input.slot=i;
					input.style.textAlign="left";
					input.type="text";
					input.size="31";
					input.maxLength="31";
					input.minLength="1";
					input.pattern="^[a-zA-Z0-9]+([a-zA-Z0-9._]+[a-zA-Z0-9])?$";
 					input.title="Level.Room.Device"; 
					input.value=data.STATUS.SwITCH[j-1].Name;
					input.onblur=function(){this.hasFocus=false;};
					input.onfocus=function(){this.hasFocus=true;};
					input.onchange=function(){AJAX_Post_SwITCH('NS'+this.slot);};
						var td = document.createElement('td');
						td.appendChild(input);
						td.height="40";
							tr.appendChild(td);

					var input = document.createElement('input');
					input.className="RB";//nötig?
					input.name="val";
					input.id="VS"+i;
					input.slotname=data.STATUS.SwITCH[j-1].Name;
					input.slot=i;
					input.type="text";
					input.size="3";
					input.maxLength="3";
					input.value=data.STATUS.SwITCH[j-1].VAL;
					input.onblur=function(){this.hasFocus=false;};
					input.onfocus=function(){this.hasFocus=true;};
					input.onchange=function(){AJAX_Post_SwITCH('VS'+this.slot);};
						var td = document.createElement('td');
						td.rowSpan="2";
						td.appendChild(input);
							tr.appendChild(td);

					var input = document.createElement('input');
					input.className="RC";//nötig?
					input.name="min";
					input.id="MINS"+i;
					input.slotname=data.STATUS.SwITCH[j-1].Name;
					input.slot=i;
					input.type="button";
					input.value="MIN";
					input.onclick=function(){AJAX_Post_SwITCH('MINS'+this.slot)};
						var td = document.createElement('td');
						td.rowSpan="2";
						td.appendChild(input);
							tr.appendChild(td);

					var input = document.createElement('input');
					input.className="RC";
					input.name="val"
					input.id="RS"+i;
					input.slotname=data.STATUS.SwITCH[j-1].Name;
					input.slot=i;
					input.type="range";
					input.min=data.STATUS.SwITCH[j-1].MIN;
					input.max=data.STATUS.SwITCH[j-1].MAX;
					input.value=data.STATUS.SwITCH[j-1].VAL;

					input.oninput=function(){this.hasFocus=true;
									var Now=Math.round( ((new Date())/500) );
									if (this.LstSend != Now)
									{this.LstSend=Now;
									/*document.getElementById('CL'+this.slot).value=
									'#'+
									decimalToHex(this.value,2)+
									(document.getElementById('CL'+this.slot).value).substr(3,4);
									document.getElementById('RL'+this.slot).value=this.value;*/
									AJAX_Post_SwITCH('RS'+this.slot);
									};};
					input.onchange=function(){this.hasFocus=false;
									/*document.getElementById('CL'+this.slot).value=
									'#'+
									decimalToHex(this.value,2)+
									(document.getElementById('CL'+this.slot).value).substr(3,4);
									document.getElementById('RL'+this.slot).value=this.value;*/
									AJAX_Post_SwITCH('RS'+this.slot);
									};
						var td = document.createElement('td');
						td.rowSpan="2";
						td.appendChild(input);
						tr.appendChild(td);

					var input = document.createElement('input');
					input.className="RC"
					input.name="max";
					input.id="MAXS"+i;
					input.slotname=data.STATUS.SwITCH[j-1].Name;
					input.slot=i;
					input.type="button";
					input.value="MAX";
					input.onclick=function(){AJAX_Post_SwITCH('MAXS'+this.slot)};
						var td = document.createElement('td');
						td.rowSpan="2";
						td.appendChild(input);
							tr.appendChild(td);

					var input = document.createElement('input');
					input.className="RB";
					input.name="dim";
					input.id="DS"+i;
					input.slotname=data.STATUS.SwITCH[j-1].Name;
					input.slot=i;
					input.type="text";
					input.size="3";
					input.maxLength="3";
					input.value=data.STATUS.SwITCH[j-1].DIM;
					input.onblur=function(){this.hasFocus=false;};
					input.onfocus=function(){this.hasFocus=true;};
					input.onchange=function(){AJAX_Post_SwITCH('DS'+this.slot);};
						var td = document.createElement('td');
						td.rowSpan="2";
						td.appendChild(input);
							tr.appendChild(td);

								// add or replace row
								var y = document.getElementById("aS"+i);
								if (y) tbody.replaceChild(tr,y);
								else tbody.appendChild(tr);

// 2. Row
					var td = document.createElement('td');
					td.className="TS";
					td.id="TISTS"+i;
					td.height="40";
					var dt = new Date(data.STATUS.SwITCH[j-1].TIST*1000);
					td.innerHTML=dt.toLocaleString('de-AT');
						var tr = document.createElement('tr');
						tr.id="bS"+i;
						tr.appendChild(td);

								// add or replace row
								var y = document.getElementById("bS"+i);
								if (y) tbody.replaceChild(tr,y);
								else tbody.appendChild(tr);
					}
//------------------------------------------------- Table construction (if data) end -----------------------------------------------
				else
//------------------------------------------------- Table data sync (if data) start -----------------------------------------------
					{
					var x=document.getElementById('NOS'+i);
					//if (!x.hasFocus) x.innerHTML=data.STATUS.SwITCH[j-1].NO;
					x.innerHTML=data.STATUS.SwITCH[j-1].NO;

					var x=document.getElementById('NS'+i);
					if (!x.hasFocus) x.value=data.STATUS.SwITCH[j-1].Name;

					var x=document.getElementById('VS'+i);
					if (!x.hasFocus) x.value=data.STATUS.SwITCH[j-1].VAL;

					var x=document.getElementById('DS'+i);
					if (!x.hasFocus) x.value=data.STATUS.SwITCH[j-1].DIM;

					var x=document.getElementById('RS'+i);
					if (!x.hasFocus) {
						x.min=data.STATUS.SwITCH[j-1].MIN;
						x.max=data.STATUS.SwITCH[j-1].MAX;
						x.value=data.STATUS.SwITCH[j-1].VAL;
					}
	
					var x=document.getElementById('TISTS'+i);
					var dt = new Date(data.STATUS.SwITCH[j-1].TIST*1000);
					x.innerHTML=dt.toLocaleString('de-AT');
					}
//------------------------------------------------- Table data sync (if data) end -----------------------------------------------
				}



/*

				if (!data.STATUS.SwITCH[j-1].NO)
					{
				if (data.STATUS.SwITCH[j-1].NO==i)
else // delete unused rows, if any
					{
					// row 1
					var y = document.getElementById("aS"+i);
					if (y) tbody.removeChild(y);

					// row 2
					var y = document.getElementById("bS"+i);
					if (y) tbody.removeChild(y);
					}
*/
			//	}
			}

		div.time_stamp = (data.STATUS.TIME);

		var AkTIST = new Date();
		var DevTIST = new Date(data.STATUS.TIME*1000);

		// update info
		var Info = document.getElementById("NFOS");
		Info.innerHTML="Local Time: " + 
			AkTIST.toLocaleTimeString('de-AT') +
			" - Data Validity: "+ DevTIST.toLocaleTimeString('de-AT') +
			" - Device Memory: " + data.STATUS.MEMORY + " Bytes";

		setTimeout(RptPoll_SwITCH, 500);
		}
	}
}



function AJAX_Post_SwITCH(formId)
{
	var elem = document.getElementById(formId);

	var params = elem.name + "=" + encodeURIComponent(elem.value);

	if (window.XMLHttpRequest) {

		// code for IE7+, Firefox, Chrome, Opera, Safari
		TXxmlhttpSwITCH = new XMLHttpRequest();
	}

	else { 
		// code for IE6, IE5
		TXxmlhttpSwITCH=new ActiveXObject("Microsoft.XMLHTTP");
	}

	TXxmlhttpSwITCH.open("POST","/"+elem.slotname+".cgi",true);

	TXxmlhttpSwITCH.send(params);

	return TXxmlhttpSwITCH.responseText;
}



