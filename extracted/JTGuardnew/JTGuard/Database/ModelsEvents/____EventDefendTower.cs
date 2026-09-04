using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace JTGuard.Database.ModelsEvents
{
	public class ____EventDefendTower
	{
		public int ID { get; set; }
		public int PCLimit { get; set; }
		public int MinPlayer { get; set; }
		public int WorldID { get; set; }
		public int RegionIDTeamRed { get; set; }
		public int StartPosXTeamRed { get; set; }
		public int StartPosYTeamRed { get; set; }
		public int StartPosZTeamRed { get; set; }
		public int RegionIDTeamBlue { get; set; }
		public int StartPosXTeamBlue { get; set; }
		public int StartPosYTeamBlue { get; set; }
		public int StartPosZTeamBlue { get; set; }
		public int RedTowerObjID { get; set; }
		public int RedTowerRegionID { get; set; }
		public int RedTowerSpawnPosX { get; set; }
		public int RedTowerSpawnPosY { get; set; }
		public int RedTowerSpawnPosZ { get; set; }
		public int BlueTowerObjID { get; set; }
		public int BlueTowerRegionID { get; set; }
		public int BlueTowerSpawnPosX { get; set; }
		public int BlueTowerSpawnPosY { get; set; }
		public int BlueTowerSpawnPosZ { get; set; }
		public int WinnerRewardID { get; set; }
		public string WinnerRewardName { get; set; }
		public int WinnerRewardQuantity { get; set; }
		public int LoserRewardID { get; set; }
		public string LoserRewardName { get; set; }
		public int LoserRewardQuantity { get; set; }
		public int KillRewardID { get; set; }
		public string KillRewardName { get; set; }
		public int KillRewardQuantity { get; set; }
	}
}