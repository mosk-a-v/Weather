using DevExpress.Xpo;
using DevExpress.Xpo.DB;
using DevExpress.Xpo.Metadata;
using System;
using System.Collections.Generic;
using System.Configuration;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Common {
    public class DBProvider {
        IDataStore raspperyStore;
        IDataLayer raspperyDataLayer;
        IDataStore localStore;
        IDataLayer localDataLayer;

        public void InitializeRaspperyDB(string connectionString) {
            raspperyStore = XpoDefault.GetConnectionProvider(connectionString, AutoCreateOption.SchemaAlreadyExists);
            var dict = new ReflectionDictionary();
            dict.CollectClassInfos(typeof(TemperatureValue).Assembly);
            raspperyDataLayer = new ThreadSafeDataLayer(dict, raspperyStore);
        }
        public void InitializeLocalDB(string connectionString) {
            localStore = XpoDefault.GetConnectionProvider(connectionString, AutoCreateOption.DatabaseAndSchema);
            var dict = new ReflectionDictionary();
            dict.CollectClassInfos(typeof(TemperatureValue).Assembly);
            localDataLayer = new ThreadSafeDataLayer(dict, localStore);
        }
        public void DropLocalDables() {
            var dict = new ReflectionDictionary();
            dict.CollectClassInfos(typeof(TemperatureValue).Assembly);
            using(var dl = new SimpleDataLayer(dict, localStore)) {
                using(var localUOW = new UnitOfWork(dl)) {
                    var tables = ((ConnectionProviderSql)dl.ConnectionProvider).GetStorageTablesList(false);
                    foreach(var tableName in tables) {
                        localUOW.ExecuteScalar($"DROP TABLE {tableName};");
                    }
                    localUOW.UpdateSchema();
                }
            }
        }
        public UnitOfWork GetRaspperyUnitOwWork() {
            return new UnitOfWork(raspperyDataLayer);
        }
        public UnitOfWork GetLocalUnitOwWork() {
            return new UnitOfWork(localDataLayer);
        }
    }
}
