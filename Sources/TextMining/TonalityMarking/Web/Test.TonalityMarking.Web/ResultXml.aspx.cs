using System;
using System.Collections.Generic;
using System.Configuration;
using System.IO;
using System.Linq;
using System.Text;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;
using System.Xml;
using System.Xml.Xsl;
using TonalityMarking;

namespace Test.TonalityMarking.Web
{
    /// <summary>
    /// 
    /// </summary>
    public partial class ResultXml : Page
    {
        protected void Page_Load(object sender, EventArgs e)
        {
            ToXml( this.TonalityMarkingOutputResult );

            this.TonalityMarkingOutputResult = null;
        }

        private TonalityMarkingOutputResult TonalityMarkingOutputResult
        {
            get { return ((TonalityMarkingOutputResult) Session[ "TonalityMarkingOutputResult" ]); }
            set 
            {
                Session.Remove( "TonalityMarkingOutputResult" );
                if ( value != null )
                    Session[ "TonalityMarkingOutputResult" ] = value; 
            }
        }
        private void ToXml( TonalityMarkingOutputResult result )
        {
            if ( result == null ) throw (new ArgumentNullException("result"));

            Response.Cache.SetCacheability( HttpCacheability.NoCache );
            Response.ContentType = "text/xml";
            Response.Write( result.OutputXml );
            Response.Flush();
            Response.End();
        }
    }
}
