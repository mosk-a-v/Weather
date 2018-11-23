using System;
using System.Configuration;
using System.Windows.Forms;
using System.Linq;
using System.Collections.Generic;
using System.Text;
using System.IO;

namespace Temperature {
    static class Program {
        [STAThread]
        static void Main() {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            var localHistory = new LocalHistoryService();
            var weatherService = new HistoryService(ConfigurationManager.AppSettings["HistoryService"]);
            var localHistoryData = localHistory.GetLocalHistory();
            var globalHistoryData = weatherService.GetHistoryFrom(2018, 11, 03);
            MatrixGenerator.OutToCSVFile(localHistoryData, globalHistoryData);
            Application.Run(new Form1(localHistoryData, globalHistoryData));
        }
        
    }
}
