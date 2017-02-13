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
using OpinionMining;

namespace Test.OpinionMining.Web
{
    public partial class ResultXml : PageBase
    {
        protected void Page_Load(object sender, EventArgs e)
        {
            XmlToResponse( this.CurrentOpinionMiningOutputResult );

            this.CurrentOpinionMiningOutputResult = null;
        }
    }
}
