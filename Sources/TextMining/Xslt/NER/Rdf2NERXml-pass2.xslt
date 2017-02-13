<?xml version="1.0"?>

<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">
  <xsl:output method="xml" indent="no" omit-xml-declaration="yes" />

  <xsl:template match="/">
    <xsl:element name="NER">
      <xsl:apply-templates />
    </xsl:element>
  </xsl:template>

  <xsl:template match="*[ node() ]">
    <xsl:apply-templates select="node()" />
  </xsl:template>

  <xsl:template match="NAME" mode="in-ner">
    <xsl:element name="NAME">      
      <xsl:apply-templates select="node()" mode="in-ner" />
    </xsl:element>
  </xsl:template>
  <xsl:template match="NAME[ node() ]">
    <xsl:element name="NAME">
      <xsl:apply-templates select="node()" mode="in-ner" />
    </xsl:element>
  </xsl:template>

  <xsl:template match="ORG" mode="in-ner">
    <xsl:element name="ORG">
      <xsl:apply-templates select="node()" mode="in-ner" />
    </xsl:element>
  </xsl:template>
  <xsl:template match="ORG[ node() ]">
    <xsl:element name="ORG">
      <xsl:apply-templates select="node()" mode="in-ner" />
    </xsl:element>
  </xsl:template>  

  <xsl:template match="GEO" mode="in-ner">
    <xsl:element name="GEO">
      <xsl:apply-templates select="node()" mode="in-ner" />
    </xsl:element>
  </xsl:template>
  <xsl:template match="GEO[ node() ]">
    <xsl:element name="GEO">
      <xsl:apply-templates select="node()" mode="in-ner" />
    </xsl:element>
  </xsl:template>

  <xsl:template match="ENTR" mode="in-ner">
    <xsl:element name="ENTR">
      <xsl:apply-templates select="node()" mode="in-ner" />
    </xsl:element>
  </xsl:template>
  <xsl:template match="ENTR[ node() ]">
    <xsl:element name="ENTR">
      <xsl:apply-templates select="node()" mode="in-ner" />
    </xsl:element>
  </xsl:template>

  <xsl:template match="PROD" mode="in-ner">
    <xsl:element name="PROD">
      <xsl:apply-templates select="node()" mode="in-ner" />
    </xsl:element>    
  </xsl:template>
  <xsl:template match="PROD[ node() ]">
    <xsl:element name="PROD">
      <xsl:apply-templates select="node()" mode="in-ner" />
    </xsl:element>
  </xsl:template>

  <xsl:template match="text()">
    <xsl:value-of select="." />
  </xsl:template>

  <xsl:template match="text()" mode="in-ner">
    <!--<xsl:value-of select="normalize-space()" />-->
    <xsl:call-template name="trim-start">
      <xsl:with-param name="str" select="." />
    </xsl:call-template>
  </xsl:template>

  <xsl:template name="trim-start">
    <xsl:param name="str" />

    <xsl:variable name="first" select="substring($str, 1, 1)"/>
    <xsl:variable name="tail" select="substring($str, 2)"/>

    <xsl:choose>
      <xsl:when test="(string-length($str) > 0) and (string-length(normalize-space($first)) = 0)">
        <xsl:call-template name="trim-start">
          <xsl:with-param name="str" select="$tail"/>
        </xsl:call-template>
      </xsl:when>
      <xsl:otherwise>
        <xsl:value-of select="$str"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>
  
</xsl:stylesheet>
