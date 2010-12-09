<?xml version="1.0" encoding="iso-8859-1"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:output method="html" encoding="iso-8859-1" doctype-system="http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd" doctype-public="-//W3C//DTD XHTML 1.1//EN"/>

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
	<html>
		<head>
			<title>agor Itemlog</title>
			<style type="text/css">
			.tooltip {
				opacity: 1;
				position: absolute;
				display: none;
				background-color: #707070;
				padding: 10px;
			}
			</style>
			<script src="tooltip.js"></script>
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
					<xsl:if test="@mode = '1'">
						solditems[n] = new Array(2);
						solditems[n]['id'] = <xsl:value-of select="@id"/>;
						solditems[n]['name'] = "<xsl:value-of select="@name"/>";
						n++;
					</xsl:if>
				</xsl:for-each>
				
				var line = "";

				function showAll() {
					var noresult = document.getElementById("noresult");
					noresult.style.display = "none";
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
					var noresult = document.getElementById("noresult");
					noresult.style.display = "none";
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
					var noresult = document.getElementById("noresult");
					noresult.style.display = "none";
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
						var noresult = document.getElementById("noresult");
						noresult.style.display = "none";

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

						if(searchresults == 0) {
							var noresult = document.getElementById("noresult");
							noresult.style.display = "block";
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
			<xsl:for-each select="/itemlog/item">
				<div id="noresult" style="display: none;">Your search returned no result!</div>
				<div>
				<xsl:attribute name="id">l<xsl:value-of select="@id"/></xsl:attribute>
				<xsl:if test="@mode = '1'">
					<xsl:attribute name="style">display: none;</xsl:attribute>
				</xsl:if>
				<xsl:choose>
					<!-- Magic -->
					<xsl:when test="@quality = '4'">
						<div style="display: block; float: left; width: 150px; margin-right: 10px; text-align: right;">
							<xsl:value-of select="@time"/>
						</div>
						<div style="display: block; float: left; width: 120px; margin-right: 10px; margin-left: 10px;">
							<xsl:value-of select="@char"/>
 						</div>
						<div style="display: block; float: left; width: 50px; margin-right: 10px; margin-left: 10px;">
							<xsl:choose>
								<xsl:when test="@mode = '0'">
									Kept
								</xsl:when>
								<xsl:when test="@mode = '1'">
									Sold
								</xsl:when>
							</xsl:choose>
						</div>
						<span style="color: #4850B8; font-weight: bold" onMouseOut="hideTT()">
							<xsl:attribute name="onMouseOver">showTT('<xsl:value-of select="@id"/>')</xsl:attribute>
							<xsl:value-of select="@name"/>
							<xsl:if test="@ethereal = '1'">
								(Ethereal)
							</xsl:if>
						</span>
						<br/>
						<div class="tooltip">
							<xsl:attribute name="id"><xsl:value-of select="@id"/></xsl:attribute>
							<span style="color: #4850B8; font-weight: bold">
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
					</xsl:when>
					<!-- Set -->
					<xsl:when test="@quality = '5'">
						<div style="display: block; float: left; width: 150px; margin-right: 10px; text-align: right;">
							<xsl:value-of select="@time"/>
						</div>
						<div style="display: block; float: left; width: 120px; margin-right: 10px; margin-left: 10px;">
							<xsl:value-of select="@char"/>
 						</div>
						<div style="display: block; float: left; width: 50px; margin-right: 10px; margin-left: 10px;">
							<xsl:choose>
								<xsl:when test="@mode = '0'">
									Kept
								</xsl:when>
								<xsl:when test="@mode = '1'">
									Sold
								</xsl:when>
							</xsl:choose>
						</div>
						<span style="color: #00C408; font-weight: bold" onMouseOut="hideTT()">
							<xsl:attribute name="onMouseOver">showTT('<xsl:value-of select="@id"/>')</xsl:attribute>
							<xsl:value-of select="@name"/>
							<xsl:if test="@ethereal = '1'">
								(Ethereal)
							</xsl:if>
						</span>
						<br/>
						<div class="tooltip">
							<xsl:attribute name="id"><xsl:value-of select="@id"/></xsl:attribute>
							<span style="color: #00C408; font-weight: bold">
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
					</xsl:when>
					<!-- Rare -->
					<xsl:when test="@quality = '6'">
						<div style="display: block; float: left; width: 150px; margin-right: 10px; text-align: right;">
							<xsl:value-of select="@time"/>
						</div>
						<div style="display: block; float: left; width: 120px; margin-right: 10px; margin-left: 10px;">
							<xsl:value-of select="@char"/>
 						</div>
						<div style="display: block; float: left; width: 50px; margin-right: 10px; margin-left: 10px;">
							<xsl:choose>
								<xsl:when test="@mode = '0'">
									Kept
								</xsl:when>
								<xsl:when test="@mode = '1'">
									Sold
								</xsl:when>
							</xsl:choose>
						</div>
						<span style="color: #E0E000; font-weight: bold" onMouseOut="hideTT()">
							<xsl:attribute name="onMouseOver">showTT('<xsl:value-of select="@id"/>')</xsl:attribute>
							<xsl:value-of select="@name"/>
							<xsl:if test="@ethereal = '1'">
								(Ethereal)
							</xsl:if>
						</span>
						<br/>
						<div class="tooltip">
							<xsl:attribute name="id"><xsl:value-of select="@id"/></xsl:attribute>
							<span style="color: #E0E000; font-weight: bold">
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
					</xsl:when>
					<!-- Unique -->
					<xsl:when test="@quality = '7'">
						<div style="display: block; float: left; width: 150px; margin-right: 10px; text-align: right;">
							<xsl:value-of select="@time"/>
						</div>
						<div style="display: block; float: left; width: 120px; margin-right: 10px; margin-left: 10px;">
							<xsl:value-of select="@char"/>
 						</div>
						<div style="display: block; float: left; width: 50px; margin-right: 10px; margin-left: 10px;">
							<xsl:choose>
								<xsl:when test="@mode = '0'">
									Kept
								</xsl:when>
								<xsl:when test="@mode = '1'">
									Sold
								</xsl:when>
							</xsl:choose>
						</div>
						<span style="color: #908858; font-weight: bold" onMouseOut="hideTT()">
							<xsl:attribute name="onMouseOver">showTT('<xsl:value-of select="@id"/>')</xsl:attribute>
							<xsl:value-of select="@name"/>
							<xsl:if test="@ethereal = '1'">
								(Ethereal)
							</xsl:if>
						</span>
						<br/>
						<div class="tooltip">
							<xsl:attribute name="id"><xsl:value-of select="@id"/></xsl:attribute>
							<span style="color: #908858; font-weight: bold">
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
					</xsl:when>
					<!-- Default -->
					<xsl:otherwise>
						<div style="display: block; float: left; width: 150px; margin-right: 10px; text-align: right;">
							<xsl:value-of select="@time"/>
						</div>
						<div style="display: block; float: left; width: 120px; margin-right: 10px; margin-left: 10px;">
							<xsl:value-of select="@char"/>
 						</div>
						<div style="display: block; float: left; width: 50px; margin-right: 10px; margin-left: 10px;">
							<xsl:choose>
								<xsl:when test="@mode = '0'">
									Kept
								</xsl:when>
								<xsl:when test="@mode = '1'">
									Sold
								</xsl:when>
							</xsl:choose>
						</div>
						<span style="color: #000000; font-weight: bold" onMouseOut="hideTT()">
							<xsl:attribute name="onMouseOver">showTT('<xsl:value-of select="@id"/>')</xsl:attribute>
							<xsl:value-of select="@name"/>
							<xsl:if test="@ethereal = '1'">
								(Ethereal)
							</xsl:if>
						</span>
						<br/>
						<div class="tooltip">
							<xsl:attribute name="id"><xsl:value-of select="@id"/></xsl:attribute>
							<span style="color: #ffffff; font-weight: bold">
								<xsl:value-of select="@name"/>
								<xsl:if test="@ethereal = '1'">
									(Ethereal)
								</xsl:if>
							</span>
							<xsl:if test="substring-after(@name, ' ') != 'Rune'">
								<br/>
								<span style="color: #ffffff">
									<xsl:call-template name="break-replace">
										<xsl:with-param name="value" select="." />
									</xsl:call-template>
								</span>
							</xsl:if>
						</div>
					</xsl:otherwise>
				</xsl:choose>
			</div>
			</xsl:for-each>
		</body>
	</html>
</xsl:template>

</xsl:stylesheet>