using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Dapper;
using JTGuard.Database.Model;
using JTGuard.Database.Models;
using JTGuard.Database.RankModels;
using Microsoft.Data.SqlClient;

namespace JTGuard.ServerManagers
{
    public class RankManager
    {
        public static Dictionary<int, _SilkRank> m_SilkRank { get; set; } = new Dictionary<int, _SilkRank>();

        public static async Task Initialize()
        {
            await LoadSilkRank();

        }
        public static async Task LoadSilkRank()
        {
            try
            {
                m_SilkRank.Clear();
                // Diğer önbellekleri de temizleyin
                // Örnek: m_AnotherReferenceTable.Clear();

                using (var connection = new SqlConnection(Program.Connectionstring))
                {
                    await connection.OpenAsync();

                    // İlk sorgu: _ActiveTitleColors tablosu
                    string query1 = "SELECT * FROM _SilkRank with (nolock)";
                    var result1 = await connection.QueryAsync<_SilkRank>(query1);
                    foreach (var item in result1)
                    {
                        if (!m_SilkRank.ContainsKey(item.CharID))
                            m_SilkRank.TryAdd(item.CharID, item);
                    }
                    //Console.WriteLine("LoadSilkRank loaded into cache. Total count: " + m_SilkRank.Count);
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Error LoadSilkRank tables: {ex.Message}");
            }
        }
    }
}
