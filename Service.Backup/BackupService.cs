using System.Collections.Generic;
using System.Configuration;
using DevExpress.Xpo;
using DevExpress.Xpo.DB;
using DevExpress.Xpo.Metadata;
using Temperature;
using System.Linq;

namespace Service.Backup {
    public class BackupService {
        readonly IDataStore baseStore;
        readonly IDataStore backupStore;
        readonly IDataLayer baseDataLayer;
        readonly IDataLayer backupDataLayer;
        public BackupService() {
            this.baseStore = XpoDefault.GetConnectionProvider(ConfigurationManager.ConnectionStrings["Weather"].ConnectionString, AutoCreateOption.SchemaAlreadyExists);
            var dict = new ReflectionDictionary();
            dict.CollectClassInfos(typeof(TemperatureValue).Assembly);
            this.baseDataLayer = new ThreadSafeDataLayer(dict, baseStore);
            this.backupStore = XpoDefault.GetConnectionProvider(ConfigurationManager.ConnectionStrings["Backup"].ConnectionString, AutoCreateOption.DatabaseAndSchema);
            dict = new ReflectionDictionary();
            dict.CollectClassInfos(typeof(TemperatureValue).Assembly);
            this.backupDataLayer = new ThreadSafeDataLayer(dict, backupStore);
            using(var dl = new SimpleDataLayer(dict, backupStore)) {
                using(var backupUOW = new UnitOfWork(dl)) {
                    var tables = ((ConnectionProviderSql)dl.ConnectionProvider).GetStorageTablesList(false);
                    foreach(var tableName in tables) {
                        backupUOW.ExecuteScalar($"DROP TABLE {tableName};");
                    }
                    backupUOW.UpdateSchema();
                }
            }
        }
        public void CopyAll() {
            using(var baseUOW = new UnitOfWork(baseDataLayer)) {
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
            using(var baseUOW = new UnitOfWork(baseDataLayer)) {
                values = new XPQuery<TemperatureValue>(baseUOW)
                   .Where(value => ids.Contains(value.Id))
                   .ToList();
            }
            using(var backupUOW = new UnitOfWork(backupDataLayer)) {
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
