<?xml version="1.0"?>

<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">
  <xsl:output method="xml" indent="yes" omit-xml-declaration="yes" />
 
  <xsl:template match="/">
    <text>
      <xsl:apply-templates />
    </text>
  </xsl:template>

  <xsl:template match="*[node()]">
    <xsl:choose>

      <xsl:when test="@SNTFINAL">
        <xsl:choose>
          <xsl:when test="@INQUIRYINITIAL='TRUE'">

            <xsl:call-template name="create-inquiry-element" />
            
          </xsl:when>
          <xsl:otherwise>

            <xsl:call-template name="create-tonality-element" />
            
          </xsl:otherwise>
        </xsl:choose>
      </xsl:when>

      <xsl:when test="@INQUIRYINITIAL='TRUE'">
        <xsl:choose>
          <xsl:when test="ancestor::*[ @SNTFINAL ]">
            <xsl:call-template name="create-nested-inquiry-element" />
          </xsl:when>
          <xsl:otherwise>
            <xsl:call-template name="create-inquiry-element" />
          </xsl:otherwise>
        </xsl:choose>        
      </xsl:when>

      <xsl:otherwise>
        <xsl:apply-templates select="node()"/>
      </xsl:otherwise>
      
    </xsl:choose>
  </xsl:template>

  <!--субъект-->
  <xsl:template match="node()[ @ISSUBJECT and not(@INQUIRYINITIAL) ]">
    <xsl:choose>
      <xsl:when test="not(ancestor::SENT//*[ @ISSUBJECT and @INQUIRYINITIAL ])">
        
        <xsl:call-template name="create-subject-element" />
        
      </xsl:when>
      <xsl:otherwise>
        <xsl:apply-templates select="node()" />
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <!--прямая и косвенная речь-->
  <xsl:template match="node()[ @INDIRECTSPEECH-BEGIN ]">
    <xsl:text disable-output-escaping="yes">&lt;indirect-speech&gt;</xsl:text>
    <xsl:apply-templates select="node()"/>
    <xsl:if test="@INDIRECTSPEECH-END">
      <xsl:text disable-output-escaping="yes">&lt;/indirect-speech&gt;</xsl:text>
    </xsl:if>
  </xsl:template>
  <xsl:template match="node()[ @INDIRECTSPEECH-END ]">
    <xsl:if test="@INDIRECTSPEECH-BEGIN">
      <xsl:text disable-output-escaping="yes">&lt;indirect-speech&gt;</xsl:text>
    </xsl:if>
    <xsl:apply-templates select="node()"/>
    <xsl:text disable-output-escaping="yes">&lt;/indirect-speech&gt;</xsl:text>
  </xsl:template>
  <xsl:template match="node()[ @DIRECTSPEECH-BEGIN ]">
    <xsl:text disable-output-escaping="yes">&lt;direct-speech&gt;</xsl:text>
    <xsl:apply-templates select="node()"/>
    <xsl:if test="@DIRECTSPEECH-END">
      <xsl:text disable-output-escaping="yes">&lt;/direct-speech&gt;</xsl:text>
    </xsl:if>
  </xsl:template>
  <xsl:template match="node()[ @DIRECTSPEECH-END ]">
    <xsl:if test="@DIRECTSPEECH-BEGIN">
      <xsl:text disable-output-escaping="yes">&lt;direct-speech&gt;</xsl:text>
    </xsl:if>
    <xsl:apply-templates select="node()"/>
    <xsl:text disable-output-escaping="yes">&lt;/direct-speech&gt;</xsl:text>
  </xsl:template>

  <!--выделение анафории-->
  <xsl:template match="node()[ @LINK and @FULLNAME and (local-name()='ENTITY' or local-name()='CET') ]">
    <xsl:element name="anaphoria">
      <xsl:attribute name="value">
        <xsl:value-of select="@FULLNAME"/>
      </xsl:attribute>
      <xsl:apply-templates select="node()" />
    </xsl:element>
  </xsl:template>

  <xsl:template match="text()">
    <xsl:call-template name="output-text">
      <xsl:with-param name="txt" select="normalize-space()" />
    </xsl:call-template>
  </xsl:template>
  
  <xsl:template match="*" priority="-9" />

  <xsl:template name="output-text">
    <xsl:param name="txt" />

    <xsl:if test="$txt != ''">
      <xsl:if test="$txt != ',' and $txt != ':' and $txt != '.' and $txt != '-' and $txt != ';' and $txt != '[' and $txt != ']' and $txt != '(' and $txt != ')'">
        <xsl:text> </xsl:text>
      </xsl:if>
      <xsl:value-of select="$txt" />     
    </xsl:if>
  </xsl:template>

  <xsl:template name="create-subject-element">
    <xsl:element name="subject">
      <xsl:call-template name="create-attribute-by-SNTFINAL" />
      <xsl:call-template name="create-attribute-by-FRTFINAL" />

      <xsl:text> </xsl:text>
      <xsl:value-of select="normalize-space()"/>
      <!--<xsl:apply-templates select="node()"/>-->
    </xsl:element>
  </xsl:template>

  <xsl:template name="create-inquiry-element">
    <xsl:element name="inquiry">
      <xsl:call-template name="create-attribute-by-SNTFINAL" />
      <xsl:call-template name="create-attribute-by-FRTFINAL" />

      <xsl:text> </xsl:text>
      <xsl:value-of select="normalize-space()"/>
    </xsl:element>
  </xsl:template>
  
  <xsl:template name="create-nested-inquiry-element">
    <xsl:element name="inquiry">
      <xsl:text> </xsl:text>
      <xsl:value-of select="normalize-space()"/>
    </xsl:element>
  </xsl:template>
  
  <xsl:template name="create-tonality-element">
    <xsl:element name="tonality">
      <xsl:call-template name="create-attribute-by-SNTFINAL" />
      <xsl:call-template name="create-attribute-by-FRTFINAL" />

      <xsl:apply-templates select="node()"/>
    </xsl:element>
  </xsl:template>

  <xsl:template name="create-attribute-by-SNTFINAL">
    <xsl:if test="@SNTFINAL">
      <xsl:attribute name="snt">
        <xsl:call-template name="SNTFINAL-to-text" />
      </xsl:attribute>
    </xsl:if>
  </xsl:template>
  
  <xsl:template name="create-attribute-by-FRTFINAL">
    <xsl:if test="@FRTFINAL != 0">
      <xsl:attribute name="frt">
        <xsl:value-of select="@FRTFINAL"/>
      </xsl:attribute>
    </xsl:if>
  </xsl:template>

  <xsl:template name="SNTFINAL-to-text">
    <xsl:value-of select="@SNTFINAL"/>
    <!--<xsl:choose>
      <xsl:when test="@SNTFINAL='nPos'">Pos</xsl:when>
      <xsl:when test="@SNTFINAL='nNeg'">Neg</xsl:when>      

      <xsl:when test="contains( @SNTFINAL, 'Pos')">Pos</xsl:when>
      <xsl:when test="contains( @SNTFINAL, 'Neg')">Neg</xsl:when>
      
      <xsl:when test="contains( @SNTFINAL, 'PPos')" >PPos</xsl:when>
      <xsl:when test="contains( @SNTFINAL, 'PNeg')" >PNeg</xsl:when>
      <xsl:when test="contains( @SNTFINAL, 'Amplf')">Amplf</xsl:when>
      <xsl:otherwise />
    </xsl:choose>-->
  </xsl:template>
  
</xsl:stylesheet>