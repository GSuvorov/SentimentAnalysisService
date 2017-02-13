<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet version="1.0"
                xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:msxsl="urn:schemas-microsoft-com:xslt"
                exclude-result-prefixes="msxsl"
                >
  <xsl:output method="html" indent="yes"/>

  <xsl:variable name="LEVEL_INDENT">30</xsl:variable>

  <xsl:include href="FinalTonality.xslt" />

  <xsl:template match="/">    
    <div>
      <style type="text/css">a img { border: 0; }</style>
      <xsl:apply-templates />
    </div>
  </xsl:template>
  
  <xsl:template match="node()[ local-name() = 'post' ]">
    <xsl:element name="div">
      <xsl:attribute name="style">
        <xsl:text>padding-left: </xsl:text>
        <xsl:value-of select="$LEVEL_INDENT * number(./lvl/text())" />
        <xsl:text>px; </xsl:text>        
      </xsl:attribute>
      
      <div style="border: 1px solid #E7E7E7; margin: 2px;">
        <xsl:if test="normalize-space(./title/text()) != ''">          
          <h3>
            <span style="font-size: x-small; color: silver;">[title]: </span>
            <xsl:value-of select="./title/text()" />
          </h3> 
        </xsl:if>
        <xsl:if test="./child::* or normalize-space(./body/text()) != ''">
          <xsl:element name="span">
            <xsl:attribute name="style">
              <xsl:text>font-size: x-small; color: silver;</xsl:text>
            </xsl:attribute>
            <xsl:attribute name="title">
              <xsl:text>level: </xsl:text><xsl:value-of select="number(./lvl/text())" />
            </xsl:attribute>
            [<xsl:value-of select="number(./lvl/text())" />]
          </xsl:element>        
          <xsl:apply-templates select="./body/node()" />
          <br/>
        </xsl:if>
        <!--
        <span style="font-size: x-small; color: silver;">[autor, date]: </span>
        <span style="font-size: x-small;">
          <span>
            <xsl:element name="a">
              <xsl:attribute name="href">
                <xsl:value-of select="./author/@authorprofile" disable-output-escaping="yes" />
              </xsl:attribute>
              <xsl:attribute name="target">_blank</xsl:attribute>

              <xsl:element name="img">
                <xsl:attribute name="src">
                  <xsl:value-of select="./author/@authoravatar" disable-output-escaping="yes" />
                </xsl:attribute>
              </xsl:element>
            </xsl:element>

            <xsl:element name="a">
              <xsl:attribute name="href">
                <xsl:value-of select="./author/@authorhome" disable-output-escaping="yes" />                
              </xsl:attribute>
              <xsl:attribute name="target">_blank</xsl:attribute>
              <b>
                <xsl:value-of select="./author/@authorname" disable-output-escaping="yes" />
              </b>
            </xsl:element>
          </span>
          <xsl:text>, </xsl:text>
          <xsl:value-of select="msxsl:format-date(./modify/text(), 'dd.MM.yyyy')" disable-output-escaping="yes" /><xsl:text> </xsl:text>
          <xsl:value-of select="msxsl:format-time(./modify/text(), 'HH:mm:ss')" disable-output-escaping="yes" />
        </span>
        -->
      </div>
    
    </xsl:element>
  </xsl:template>

  <xsl:template match="node()[ local-name() = 'filters' ]" />  

</xsl:stylesheet>
