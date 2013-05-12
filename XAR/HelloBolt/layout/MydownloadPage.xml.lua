function OnPageInit(self)
	self:SetObjPos(0,0,"father.width","father.height")
	--然后还要载入保存的任务列表
	
	local list = self:GetControlObject("listbox")
	local data = {}
	data.Name = "音乐"
	list:AddItem(data)
	data.Name = "电影"
	list:AddItem(data)
	data.Name = "游戏"
	list:AddItem(data)
	data.Name = "图书"
	list:AddItem(data)

	local coolClientProxy = XLGetObject('CoolDown.CoolClient.Proxy')
	coolClientProxy:RunClientAsync()
	list:UpdateUI()
end

function OnCronClick(self)
	local templateMananger = XLGetObject("Xunlei.UIEngine.TemplateManager")				
	local hostwndManager = XLGetObject("Xunlei.UIEngine.HostWndManager")
	local parent = hostwndManager:GetHostWnd("MainFrame")
	local tab = parent:GetUserData()
	local parentHandle = parent:GetWndHandle()
	
	local menuTreeTemplate = templateMananger:GetTemplate("MydownloadPage.CronMenu","ObjectTreeTemplate")
	local menuTree = menuTreeTemplate:CreateInstance("Thunder.CronMenu.Instance")
	local menuHostWndTemplate = templateMananger:GetTemplate("Thunder.MainMenuHostwnd", "HostWndTemplate")
	local menuHostWnd = menuHostWndTemplate:CreateInstance("Thunder.MenuHostWnd.Instance")
	

	menuHostWnd:BindUIObjectTree(menuTree)
	local menuContext = menuTree:GetUIObject("root")
	if tab.Cron ~= nil then
		menuTree:GetUIObject(tab.Cron):SetText("取消")
	end
	--获取父窗口的宽度还未实现，更无法实现窗口可变
	local left,top,right,bottom = self:GetOwnerControl():GetControlObject("bottombkg"):GetObjPos()
	local x,y = parent:ClientPtToScreenPt(left,top)
	menuHostWnd:TrackPopupMenu(parentHandle, x + 60, y, x + 150, y + 66)
	
	local objtreeManager = XLGetObject("Xunlei.UIEngine.TreeManager")	
	objtreeManager:DestroyTree("Thunder.CronMenu.Instance")
	hostwndManager:RemoveHostWnd("Thunder.MenuHostWnd.Instance")
	
	--XLMessageBox(self:GetOwner():GetID())
end

function OnCronMenuItemClick(self)
	local id = self:GetID()
	local text = self:GetControlObject("text")
	local treeManager = XLGetObject("Xunlei.UIEngine.TreeManager")				
	local tree = treeManager:GetUIObjectTree("MainObjectTree")
	local cronBtn = tree:GetUIObject("tabbkg"):GetControlObject("MydownloadPage"):GetControlObject("cron")
	
	local hostwndManager = XLGetObject("Xunlei.UIEngine.HostWndManager")
	local host = hostwndManager:GetHostWnd("MainFrame")
	local tab = host:GetUserData()
	
	if tab.Cron == nil then
		if id == "item.shutdown" then
			--下载后关机
		elseif id == "item.exit" then
			--下载后退出
		elseif id == "item.stopupload" then
			--下载后停止上传
		end
		tab.Cron = id
		cronBtn:SetText(text:GetText())
	else
		tab.Cron = nil
		cronBtn:SetText("计划任务")
	end
end

--listbox选中发生变化，这里用来更新按钮之类的 index从1开始
function OnListBoxSelectChanged(self,eventname,index)
	--XLMessageBox(self:GetItemByIndex(index):GetClass())
	local page = self:GetOwnerControl()
	local panel = page:GetControlObject("controlPanel")
	
	local start = panel:GetControlObject("btn.start")
	local pause = panel:GetControlObject("btn.pause")
	local delete = panel:GetControlObject("btn.delete")
	local openfolder = panel:GetControlObject("btn.openfolder")
	
	--
	start:SetEnable(true)
	pause:SetEnable(true)
	delete:SetEnable(true)
	openfolder:SetEnable(true)
end