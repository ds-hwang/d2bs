<?xml version="1.0" encoding="iso-8859-1"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:output method="html" encoding="iso-8859-1" doctype-system="http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd" doctype-public="-//W3C//DTD XHTML 1.1//EN"/>

     
<xsl:param name="attrName" select="'mode'"/>





<xsl:template name="break-replace">
      <xsl:param name="value" />
      <xsl:choose>
         <xsl:when test="contains($value,'|')">
            <xsl:value-of select="substring-before($value,'|')" />
            <br/>
            <xsl:call-template name="break-replace">
               <xsl:with-param name="value" select="substring-after($value,'|')" />
            </xsl:call-template>
         </xsl:when>
         <xsl:otherwise>
            <xsl:value-of select="$value" />
         </xsl:otherwise>
      </xsl:choose>
</xsl:template>

<xsl:template match="itemlog">
	<xsl:variable xml:id="xslValue" name="attrValue">1</xsl:variable>
	
	<html>
		<head>
			<title>D2BS Itemlog</title>
			<style type="text/css">
			.tooltip {
				opacity: 1;
				position: absolute;
				display: none;
				background-color: #707070;
				padding: 10px;
			}
			</style>
			<script src="tooltip.js" />
			<script>
				var keptitems = new Array();
				var solditems = new Array();
				var i = 0;
				var n = 0;
				<xsl:for-each select="/itemlog/item">
					<xsl:if test="@mode = '0'">
						keptitems[i] = new Array(2);
						keptitems[i]['id'] = <xsl:value-of select="@id"/>;
						keptitems[i]['name'] = "<xsl:value-of select="@name"/>";
						i++;
					</xsl:if>
					<xsl:if test="@mode = '1' or @mode = '2'">
						solditems[n] = new Array(2);
						solditems[n]['id'] = <xsl:value-of select="@id"/>;
						solditems[n]['name'] = "<xsl:value-of select="@name"/>";
						n++;
					</xsl:if>
				</xsl:for-each>
				
				var line = "";

				function showAll() {
					for(i=0;i &lt; keptitems.length; i++) {
						line = document.getElementById("l" + keptitems[i]['id']);
						line.style.display = "block";
					}
					for(i=0;i &lt; solditems.length; i++) {
						line = document.getElementById("l" + solditems[i]['id']);
						line.style.display = "block";
					}
				}

				function showKept() {
					for(i=0;i &lt; keptitems.length; i++) {
						line = document.getElementById("l" + keptitems[i]['id']);
						line.style.display = "block";
					}
					for(i=0;i &lt; solditems.length; i++) {
						line = document.getElementById("l" + solditems[i]['id']);
						line.style.display = "none";
					}
				}

				function showSold() {
					for(i=0;i &lt; keptitems.length; i++) {
						line = document.getElementById("l" + keptitems[i]['id']);
						line.style.display = "none";
					}
					for(i=0;i &lt; solditems.length; i++) {
						line = document.getElementById("l" + solditems[i]['id']);
						line.style.display = "block";
					}
				}

				function showNone() {
					for(i=0;i &lt; keptitems.length; i++) {
						line = document.getElementById("l" + keptitems[i]['id']);
						line.style.display = "none";
					}
					for(i=0;i &lt; solditems.length; i++) {
						line = document.getElementById("l" + solditems[i]['id']);
						line.style.display = "none";
					}
				}
				
				function searchName(e) {
					var ENTER_KEY = 13;
					var code = "";

					if (window.event) { // IE
						code = e.keyCode;
					} else if (e.which) { // Netscape/Firefox/Opera
						code = e.which;
					}

					if(code == ENTER_KEY) {
						var input = document.getElementById("search");
						var string = input.value;
						var keptbox = document.getElementById("keptbox");
						var soldbox = document.getElementById("soldbox");
						var searchresults = 0;
						var search = eval("/" + string + "/i");
						showNone();
	
						if(keptbox.checked == true) {
							for(i=0;i &lt; keptitems.length; i++) {
								var itemname = keptitems[i]['name'];
								var result = search.test(itemname);
								if(result == true) {
									searchresults++;
									
									line = document.getElementById("l" + keptitems[i]['id']);
									line.style.display = "block";
								}
							}
						}

						if(soldbox.checked == true) {
							for(i=0;i &lt; solditems.length; i++) {
								var itemname = solditems[i]['name'];
								var result = search.test(itemname);
								if(result == true) {
									searchresults++;
						
									line = document.getElementById("l" + solditems[i]['id']);
									line.style.display = "block";
								}
							}
						}
					}
				}	
			</script>
		</head>
		<body style="font: 12px Verdana">
			<p>			 
				<input type="button" value="Show All" onClick="showAll()"/>
				<input type="button" value="Show Kept" onClick="showKept()"/>
				<input type="button" value="Show Sold" onClick="showSold()"/>
				
				<span style="padding-left: 50px;">
					Search Item Name: <input type="text" id="search" onkeypress="searchName(event)"/>
					Kept <input type="checkbox" id="keptbox" checked="checked"/>
					Sold <input type="checkbox" id="soldbox" checked="checked"/>
				</span>
			</p>
			<table>
				
				<col width="150" />
				<col width="120" />
				<col width="50" />
				<col width="50" />
				<col width="*" />
				<col width="200" />
				<tr>
					<th>Date</th>
					<th>Character</th>
					<th>Action</th>
					<th>Ilvl</th>
					<th>Item</th>
					<th>From</th>
				</tr>
			<xsl:for-each select="item">
			   <xsl:variable name="color">
			        <xsl:choose>
			           <xsl:when test="@quality = '4'">#4850B8</xsl:when>
			           <xsl:when test="@quality = '5'">#00C408</xsl:when>
			           <xsl:when test="@quality = '6'">#E0E000</xsl:when>
			           <xsl:when test="@quality = '7'">#908858</xsl:when>
			           <xsl:otherwise>#000000</xsl:otherwise>
			        </xsl:choose>
			   </xsl:variable>
				<tr>
					<xsl:attribute name="id">l<xsl:value-of select="@id"/></xsl:attribute>
					<xsl:if test="@mode = '1' or @mode = '2'">
						<xsl:attribute name="style">display: none;</xsl:attribute>
					</xsl:if>
					<td>
						<xsl:value-of select="@time"/>
					</td>
					<td>					
						<xsl:value-of select="@char"/>
					</td>
					<td>
					<xsl:choose>
						<xsl:when test="@mode = '0'">
							Kept
						</xsl:when>
						<xsl:when test="@mode = '1'">
							Sold
						</xsl:when>
						<xsl:when test="@mode = '2'">
							Drop
						</xsl:when>								
					</xsl:choose>
					</td>
					<td>
				     	ilvl: <xsl:value-of select="@ilvl" />
					</td>
					<td>
						<span style="color: {$color}; font-weight: bold" onMouseOut="hideTT()">
						<xsl:attribute name="onMouseOver">showTT('<xsl:value-of select="@id"/>')</xsl:attribute>
						<xsl:value-of select="@name"/>
						<xsl:if test="@ethereal = '1'">
							(Ethereal)
						</xsl:if>
						</span>
						<br/>
						<div class="tooltip">
						  <xsl:attribute name="id"><xsl:value-of select="@id"/></xsl:attribute>
							<span style="color: {$color}; font-weight: bold">
									<xsl:value-of select="@name"/>
									<xsl:if test="@ethereal = '1'">
										(Ethereal)
									</xsl:if>
							</span>
							<br/>
							<span style="color: #ffffff">
								<xsl:call-template name="break-replace">
							 	 <xsl:with-param name="value" select="." />
								</xsl:call-template>
							</span>
						</div>						
					</td>
					<td>
						<xsl:value-of select="@from"/>
					</td>
				</tr>
			</xsl:for-each>
			</table>				
		</body>
	</html>
</xsl:template>

</xsl:stylesheet>