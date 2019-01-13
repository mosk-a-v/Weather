using System;
using System.Configuration;
using System.Windows.Forms;
using System.Linq;
using System.Collections.Generic;
using System.Text;
using System.IO;
using Common;

namespace Temperature {
    static class Program {
        [STAThread]
        static void Main() {
            const bool WriteMatrix = false;
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            var dBProvider = new DBProvider();
            dBProvider.InitializeRaspperyDB(ConfigurationManager.ConnectionStrings["Weather"].ConnectionString);
            if(WriteMatrix) {
                MatrixGenerator.WriteControlMatrix(dBProvider);
                MatrixGenerator.WriteSettings(dBProvider);
            }
            var localHistory = new LocalHistoryService(dBProvider);
            HistoryService weatherService;
            if(!string.IsNullOrEmpty(ConfigurationManager.AppSettings["HistoryService"])) {
                weatherService = new HistoryService(new Uri(ConfigurationManager.AppSettings["HistoryService"]));
            } else {
                weatherService = new HistoryService(ConfigurationManager.AppSettings["HistoryFileName"]);
            }
            var localHistoryData = localHistory.GetLocalHistory();
            var globalHistoryData = weatherService.GetHistoryFrom(2018, 11, 03);
            MatrixGenerator.OutToCSVFile(localHistoryData, globalHistoryData);
            Application.Run(new Form1(localHistoryData, globalHistoryData));
        }

    }
}
