<?xml version="1.0" encoding="utf-8"?>

<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"
                xmlns:msxsl="urn:schemas-microsoft-com:xslt"
                xmlns:user="urn:user-scripts">

  <xsl:output method="html"/>
  <xsl:param name="start-sent-number" select="0" />
  <xsl:param name="show-sent-number" select="true()" />

  <xsl:param name="commonHighlight" select="true()"/>
  <xsl:param name="whichHighlight" select="true()"/>
  <xsl:param name="selfHighlight" select="true()"/>
  <xsl:param name="thatHighlight" select="true()"/>
  <xsl:param name="aliasHighlight" select="true()"/>
  <xsl:param name="whereProperHighlight" select="true()"/>
  <xsl:param name="whoHighlight" select="true()"/>
  <xsl:param name="whHighlight" select="true()"/>
  <xsl:param name="suchAsHighlight" select="true()"/>
  <xsl:param name="firstPersonHighlight" select="true()"/>

  <msxsl:script language="C#" implements-prefix="user">
    <![CDATA[
        public string SplitLongString(string stringToSplit)
        {
            return stringToSplit.Replace(",", ", "); // <wbr/>
        }
      ]]>
  </msxsl:script>


  <xsl:template match="/">

    <!--Courier, Tahoma-->
    <div style="font-family: Tahoma; font-size: 10pt; margin-bottom: 2em;">
      <xsl:choose>
        <xsl:when test="$show-sent-number">
          <div style="margin-left: 1em;">
            <span style="color: maroon;">&lt;TEXT&gt;</span>
            <br/>
            <xsl:for-each select="TEXT/SENT">
              <xsl:element name="span">
                <xsl:attribute name="title">
                  sent #<xsl:value-of select="number($start-sent-number) + position()"/>
                </xsl:attribute>
                <xsl:attribute name="style">font-weight: bold;</xsl:attribute>
                (<xsl:value-of select="number($start-sent-number) + position()"/>)
              </xsl:element>

              <xsl:call-template name="apply-templates-4-sent" />
              <!--<xsl:apply-templates />-->

            </xsl:for-each>
            <span style="color: maroon;">&lt;/TEXT&gt;</span>
          </div>
        </xsl:when>
        <xsl:otherwise>
          <xsl:apply-templates />
        </xsl:otherwise>
      </xsl:choose>
    </div>

  </xsl:template>

  <xsl:template name="apply-templates-4-sent">
    <div style="margin-left: 1em;">
      <span style="color: maroon;">
        &lt;<xsl:value-of select="local-name()"/><xsl:apply-templates select="@*"/>&gt;
      </span>
      <xsl:apply-templates />
      <span style="color: maroon;">
        &lt;/<xsl:value-of select="local-name()"/>&gt;
      </span>
    </div>
  </xsl:template>

  <xsl:template match="*">
    <div style="margin-left: 1em; color: maroon;">
      &lt;<xsl:value-of select="local-name()"/><xsl:apply-templates select="@*"/>/&gt;
    </div>
  </xsl:template>

  <xsl:template match="*[node()]">
    <div style="margin-left: 1em;">
      <span style="color: maroon;">
        &lt;<xsl:value-of select="local-name()"/><xsl:apply-templates select="@*"/>&gt;
      </span>
      <xsl:apply-templates select="node()"/>
      <span style="color: maroon;">
        &lt;/<xsl:value-of select="local-name()"/>&gt;
      </span>
    </div>
  </xsl:template>

  <xsl:template match="text()">
    <xsl:choose>
      <xsl:when test="normalize-space() != '' or parent::U">
        <span style="color: silver;">
          &lt;![CDATA[<span style="color: gray;">
            <xsl:value-of select="." />
          </span>]]&gt;
        </span>
      </xsl:when>
      <xsl:otherwise>
        <xsl:value-of select="." />
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <xsl:template match="@*">
    <xsl:if test="local-name() != 'FULLNAME'">
      <span style="color: blue;">
        <xsl:text> </xsl:text>
        <xsl:value-of select="local-name()"/>="<span style="color: black;">
          <!--<xsl:value-of select="." /> <- OldVersion-->
          <xsl:value-of select="user:SplitLongString(.)"/>
        </span>"
      </span>
    </xsl:if>
  </xsl:template>

  <xsl:template match="@LINK">
    <xsl:call-template name="link-attribute">
      <xsl:with-param name="backgroundcolor">DeepPink</xsl:with-param>
    </xsl:call-template>
  </xsl:template>

  <xsl:template match="@P-LINK | @P-LINK-SUBSENT">
    <xsl:call-template name="link-attribute">
      <xsl:with-param name="backgroundcolor">LightSkyBlue</xsl:with-param>
    </xsl:call-template>
  </xsl:template>

  <xsl:template match="@D-LINK | @D-LINK-SUBSENT">
    <xsl:call-template name="link-attribute">
      <xsl:with-param name="backgroundcolor">LimeGreen</xsl:with-param>
    </xsl:call-template>
  </xsl:template>

  <xsl:template match="@CAT-LINK | @CAT-LINK-SUBSENT">
    <xsl:call-template name="link-attribute">
      <xsl:with-param name="backgroundcolor">Orange</xsl:with-param>
    </xsl:call-template>
  </xsl:template>

  <xsl:template match="@AUT-LINK">
    <xsl:call-template name="link-attribute">
      <xsl:with-param name="backgroundcolor">Brown</xsl:with-param>
    </xsl:call-template>
  </xsl:template>

  <xsl:template match="@SPEECH-LINK">
    <xsl:call-template name="link-attribute">
      <xsl:with-param name="backgroundcolor">Blue</xsl:with-param>
    </xsl:call-template>
  </xsl:template>

  <xsl:template name="link-attribute">
    <xsl:param name="backgroundcolor" />

    <xsl:text> </xsl:text>
    <xsl:element name="span">
      <xsl:attribute name="id">
        <xsl:value-of select="parent::*/@ID"/>
      </xsl:attribute>
      <xsl:attribute name="style">
        <xsl:text>color: white; border: 1px dotted #000000; font-weight: bold; background-color: </xsl:text>
        <xsl:value-of select="$backgroundcolor"/>
      </xsl:attribute>

      <span style="color: WhiteSmoke;">
        <xsl:value-of select="local-name()"/>="
      </span>
      <xsl:value-of select="." />
      <span style="color: WhiteSmoke;">"</span>

    </xsl:element>

    <xsl:text> </xsl:text>
    <xsl:element name="span">
      <xsl:attribute name="style">
        <xsl:text>color: white; border: 1px dotted #000000; font-weight: bold; background-color: </xsl:text>
        <xsl:value-of select="$backgroundcolor"/>
      </xsl:attribute>

      <span style="color: WhiteSmoke;">FULLNAME="</span>
      <xsl:value-of select="parent::*/@FULLNAME" />
      <span style="color: WhiteSmoke;">"</span>

    </xsl:element>
  </xsl:template>

</xsl:stylesheet>