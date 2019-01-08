using System.Collections.Generic;
using System.Configuration;
using DevExpress.Xpo;
using DevExpress.Xpo.DB;
using DevExpress.Xpo.Metadata;
using Temperature;
using System.Linq;
using Common;

namespace Service.Backup {
    public class BackupService {
        readonly DBProvider dBProvider;

        public BackupService() {
            this.dBProvider = new DBProvider();
            dBProvider.InitializeRaspperyDB(ConfigurationManager.ConnectionStrings["Weather"].ConnectionString);
            dBProvider.InitializeLocalDB(ConfigurationManager.ConnectionStrings["Backup"].ConnectionString);
            dBProvider.DropLocalDables();
        }
        public void CopyAll() {
            using(var baseUOW = dBProvider.GetRaspperyUnitOwWork()) {
                var idList = new XPQuery<TemperatureValue>(baseUOW)
                   .Select(item => item.Id)
                   .ToList();
                for(int batchNum = 0; batchNum <= idList.Count / 2000; batchNum++) {
                    CopyBatch(idList.Skip(batchNum * 2000).Take(2000).ToList());
                }
            }
        }
        void CopyBatch(List<int> ids) {
            List<TemperatureValue> values;
            using(var baseUOW = dBProvider.GetRaspperyUnitOwWork()) {
                values = new XPQuery<TemperatureValue>(baseUOW)
                   .Where(value => ids.Contains(value.Id))
                   .ToList();
            }
            using(var backupUOW = dBProvider.GetLocalUnitOwWork()) {
                foreach(var value in values) {
                    var newValue = new TemperatureValue(backupUOW) {
                        Time = value.Time,
                        Device = value.Device,
                        Value = value.Value
                    };
                    newValue.ClassInfo.KeyProperty.SetValue(newValue, value.Id);
                }
                backupUOW.CommitChanges();
            }
        }
    }
}
