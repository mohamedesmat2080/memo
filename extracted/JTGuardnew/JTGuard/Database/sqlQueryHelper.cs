using System;
using System.Collections.Generic;
using System.Data;
using System.Data.SqlClient;
using System.Linq;
using System.Threading.Tasks;
using Dapper;
using JTGuard.Database.Models;
using JTGuard.SessionManager;
using Microsoft.Data.SqlClient;
using static System.Collections.Specialized.BitVector32;

namespace JTGuard.Database
{
    public class SItemInfoDbRecord
    {
        public int nRefItemID;
        public byte btOptLevel;
        public string szCodeName;
        public int nItemDBID;
        public byte btAdvOptLevel;
    }

    public class sqlQueryHelper
    {
        public static async Task<List<int>> GetWhitelistAsync(string connectionString, int ServerType)
        {
            using (SqlConnection con = new SqlConnection(connectionString))
            {
                var whitelist = await con.QueryAsync<int>($"select MsgId from __Whitelist with(nolock) where ServerType = {ServerType}");
                return whitelist.ToList();
            }
        }

        public static async Task<List<int>> GetBlacklistAsync(string connectionString, int ServerType)
        {
            using (SqlConnection con = new SqlConnection(connectionString))
            {
                var blacklist = await con.QueryAsync<int>($"select MsgId from __Blacklist with(nolock) where ServerType = {ServerType}");
                return blacklist.ToList();
            }
        }
        public static async Task<bool> ValidateUserCredentials(string username, string password)
        {
            try
            {
                using (var connection = new SqlConnection(Program.Connectionstring))
                {
                    await connection.OpenAsync();

                    string query = $"EXEC [dbo].[_AuthCharLogin] @UserName='{username}', @Password='{password}'";
                    int result = await connection.QuerySingleOrDefaultAsync<int>(query);
                    return result == 1;
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Error validating user credentials: {ex.Message}");
                return false;
            }
        }
        public static async Task<string> prod_string(string query, string connectionstring)
        {
            string value = "";
            try
            {
                using (SqlConnection con = new SqlConnection(connectionstring))
                {
                    using (SqlCommand cmd = new SqlCommand(query, con))
                    {
                        await con.OpenAsync();
                        return Convert.ToString(await cmd.ExecuteScalarAsync());
                    }
                }
            }
            catch (NullReferenceException)
            {
                Console.WriteLine($"SQL Error -> Function prod_string({query}), exception catched : Null exception");
                return "";
            }
            catch (SqlException Ex)
            {
                Console.WriteLine($"SQL Error -> Function GetInt({query}), exception catched : {Ex.ToString()}");
            }
            return value;
        }
        public static async Task<int> prod_int(string query, string connectionstring)
        {
            int value = 0;
            try
            {
                using (SqlConnection con = new SqlConnection(connectionstring))
                {
                    using (SqlCommand cmd = new SqlCommand(query, con))
                    {
                        await con.OpenAsync();
                        object result = await cmd.ExecuteScalarAsync();
                        value = result != null ? Convert.ToInt32(result) : 0;
                        return value;
                    }
                }
            }
            catch (NullReferenceException)
            {
                Console.WriteLine($"SQL Error -> Function prod_int({query}), exception catched: Null exception");
                return 0;
            }
            catch (SqlException Ex)
            {
                Console.WriteLine($"SQL Error -> Function prod_int({query}), exception catched: {Ex.ToString()}");
                return 0;
            }
        }
        public static async Task CallLoggerMobKill(string Charname, int CharID, int MobID, int BRegionID, int WorldID, int MonsterClass)
        {
            using (var connection = new SqlConnection(Program.Connectionstring))
            {
                await connection.OpenAsync();
                string query = $"EXEC _LoggerMobKill '{Charname}', {CharID}, {MobID}, {BRegionID}, {WorldID}, {MonsterClass}";
                using (var command = new SqlCommand(query, connection))
                {
                    await command.ExecuteNonQueryAsync();

                }
            }
        }
        public static async Task AddItemToChest(int CharID, string ItemCodeName, int Quantity, string Type, int Plus)
        {
            try
            {
                using (var connection = new SqlConnection(Program.Connectionstring))
                {
                    await connection.OpenAsync();
                    string query = $"EXEC _AddItemToChest {CharID}, '{ItemCodeName}', {Quantity}, '{Type}', {Plus}";

                    using (var command = new SqlCommand(query, connection))
                    {
                        await command.ExecuteNonQueryAsync();

                    }
                }
            }
            catch (Exception ex)
            {
                Program.PrintInColor(ex.Message.ToString() + "addItemToChest", ConsoleColor.Red);
            }
        }
        public static async Task InsertSecondaryPasswordData(string userId, int Password, string Hwid, bool RememberPC)
        {
            try
            {
                using (var connection = new SqlConnection(Program.Connectionstring))
                {
                    await connection.OpenAsync();
                    int rememberPCValue = RememberPC ? 1 : 0;
                    string query = $"INSERT INTO _AccountSecondaryPassword (StrUserID, Password, Hwid, RememberPC) VALUES('{userId}'," +
                        $"{Password}, '{Hwid}', {rememberPCValue})";
                    var result = await connection.QuerySingleOrDefaultAsync(query);

                    return;
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Error retrieving secondary password data: {ex.Message}");
                return;
            }
        }
        public static async Task UpdateSecondaryPasswordData(string userId, int Password, bool RememberPC)
        {
            try
            {
                using (var connection = new SqlConnection(Program.Connectionstring))
                {
                    await connection.OpenAsync();
                    int rememberPCValue = RememberPC ? 1 : 0;
                    string query = $"UPDATE _AccountSecondaryPassword SET Password = {Password}, RememberPC={rememberPCValue} where StrUserID = '{userId}'";
                    var result = await connection.QuerySingleOrDefaultAsync(query);

                    return;
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Error retrieving secondary password data: {ex.Message}");
                return;
            }
        }
        public static async Task EXEC_QUERY(string query)
        {
            try
            {
                using (SqlConnection con = new SqlConnection(Program.Connectionstring))
                {
                    using (SqlCommand cmd = new SqlCommand(query, con))
                    {
                        await con.OpenAsync();
                        await cmd.ExecuteNonQueryAsync();
                    }
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.Message.ToString() + query + "ERROR QUERY EXEC");
            }
        }
        public static async Task<SecondaryPasswordData> GetSecondaryPasswordData(string userId)
        {
            try
            {
                using (var connection = new SqlConnection(Program.Connectionstring))
                {
                    await connection.OpenAsync();

                    string query = $"SELECT * FROM _AccountSecondaryPassword WHERE StrUserID = '{userId}'";
                    var result = await connection.QuerySingleOrDefaultAsync<SecondaryPasswordData>(query);

                    return result;
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Error retrieving secondary password data: {ex.Message}");
                return null;
            }
        }
    }
}
