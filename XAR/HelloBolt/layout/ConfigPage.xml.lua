function OnPageInit(self)
	self:SetObjPos(0,0,"father.width","father.height")
	local coolClientProxy = XLGetObject('CoolDown.CoolClient.Proxy')
	local runwithos = self:GetControlObject("runwithos")
	local AutoStartDownloading = self:GetControlObject("startasadd")
	local DefaultTorrentPath = self:GetControlObject("torrentfolder.input")
	local MaxParallelTask = self:GetControlObject("maxdownloadtask.input")
	local MaxDownloadSpeed = self:GetControlObject("maxdownloadspeed.input")
	local MaxUploadSpeed = self:GetControlObject("maxuploadspeed.input")
	local DefaultDownloadPath = self:GetControlObject("savefolder.input")
	local DownloadNotificationSound = self:GetControlObject("beepasfinished")
	local alertasfinished = self:GetControlObject("alertasfinished") 
	
	AutoStartDownloading:SetCheck(coolClientProxy:GetConfig('AutoStartDownloading'))
	DefaultTorrentPath:SetText(coolClientProxy:GetConfig('DefaultTorrentPath'))
	MaxParallelTask:SetText(coolClientProxy:GetConfig('MaxParallelTask'))
	MaxDownloadSpeed:SetText(coolClientProxy:GetConfig('MaxDownloadSpeed'))
	MaxUploadSpeed:SetText(coolClientProxy:GetConfig('MaxUploadSpeed'))
	DefaultDownloadPath:SetText(coolClientProxy:GetConfig('DefaultDownloadPath'))
	DownloadNotificationSound:SetCheck(coolClientProxy:GetConfig('DownloadNotificationSound'))
end

--弹出确认对话框，是否还原默认设置
function OnResetButtonClick(self)
	local templateManager = XLGetObject("Xunlei.UIEngine.TemplateManager")
	local hostWndManager = XLGetObject("Xunlei.UIEngine.HostWndManager")
	local mainWnd = hostWndManager:GetHostWnd("MainFrame")
	local modalHostWndTemplate = templateManager:GetTemplate("Thunder.MessageBox","HostWndTemplate")
	local modalHostWnd = modalHostWndTemplate:CreateInstance("Thunder.MessageBox.Instance")
	local objectTreeTemplate = templateManager:GetTemplate("Thunder.MessageBox","ObjectTreeTemplate")
	local uiObjectTree = objectTreeTemplate:CreateInstance("Thunder.MessageBox.Instance")
	modalHostWnd:BindUIObjectTree(uiObjectTree)
	
	local userData = {Title = "设置中心", Icon="bitmap.confirmmodal.warning", Content="提示:是否还原默认设置？", 
		Object = self:GetOwnerControl(), EventName = "OnConfigResetConfirm"}
	modalHostWnd:SetUserData(userData)
	modalHostWnd:DoModal(mainWnd:GetWndHandle())
	
	local objtreeManager = XLGetObject("Xunlei.UIEngine.TreeManager")	
	objtreeManager:DestroyTree("Thunder.MessageBox.Instance")
	hostWndManager:RemoveHostWnd("Thunder.MessageBox.Instance")
end

--设置发生变更的动画提示
function ConfigChangeAni(self,text)
	local xarManager = XLGetObject("Xunlei.UIEngine.XARManager")
	local xarFactory = xarManager:GetXARFactory()
	local fillObj = xarFactory:CreateUIObject("TmpFillObject", "FillObject")
	local textObj = xarFactory:CreateUIObject("TmpTextObject", "TextObject")
	local top = self:GetControlObject("top")
	
	fillObj:SetAlpha(0)
	top:AddChild(fillObj)
	top:AddChild(textObj)
	fillObj:SetObjPos2(0,0,"father.width","father.height")
	textObj:SetObjPos2("father.width - 480",0,300,"father.height")
	fillObj:SetSrcColor("color.configpage.alert")
	textObj:SetTextFontResID("font.configpage.title")
	textObj:SetTextColorResID("color.configpage.alerttext")
	textObj:SetVAlign("center")
	textObj:SetHAlign("center")
	textObj:SetText(text)
	local counter1 = SetTimer(function() fillObj:SetAlpha(fillObj:GetAlpha()+10) end, 10)
	local counter2 = SetOnceTimer(function() KillTimer(counter1) end, 500)
	local counter3 = SetOnceTimer(
		function() 
			local counter1 = SetTimer(function() fillObj:SetAlpha(fillObj:GetAlpha()-10) end,10)
			local counter2 = SetOnceTimer(function() KillTimer(counter1) top:RemoveChild(textObj) end,500)
		end, 2000)
	local counter3 = SetOnceTimer(function() top:RemoveChild(fillObj) end, 3000)
end

--还原默认设置
function OnConfigResetConfirm(self)
	local hostwndManager = XLGetObject("Xunlei.UIEngine.HostWndManager")
	local hostwnd = hostwndManager:GetHostWnd("MainFrame")
	local userData = hostwnd:GetUserData()
	local defaultConfig = { runwithos = true, startasadd = true, torrentfolder = "C:\\cd\\", maxdownloadtask = 10, maxdownloadspeed = "0",
			maxuploadspeed = "1024", savefolder = "C:\\downloads\\", beepasfinished = true, alertasfinished = true }
	--userData.Config = defaultConfig
	
	local runwithos = self:GetControlObject("runwithos")
	local startasadd = self:GetControlObject("startasadd")
	local torrentfolder = self:GetControlObject("torrentfolder.input")
	local maxdownloadtask = self:GetControlObject("maxdownloadtask.input")
	local maxdownloadspeed = self:GetControlObject("maxdownloadspeed.input")
	local maxuploadspeed = self:GetControlObject("maxuploadspeed.input")
	local savefolder = self:GetControlObject("savefolder.input")
	local beepasfinished = self:GetControlObject("beepasfinished")
	local alertasfinished = self:GetControlObject("alertasfinished")
	
	local coolClientProxy = XLGetObject('CoolDown.CoolClient.Proxy')
	
	runwithos:SetCheck(true)
	startasadd:SetCheck(true)
	torrentfolder:SetText(coolClientProxy:GetConfig('DefaultTorrentPath'))
	maxdownloadtask:SetText(5)
	maxdownloadspeed:SetText(512)
	maxuploadspeed:SetText(512)
	savefolder:SetText("E:\\download\\")
	beepasfinished:SetCheck(true)
	
	self:ConfigChangeAni("已恢复默认设置")
	--XLMessageBox("succ")
end

function GetNewConfig(self)
	local newConfig = {}
	local runwithos = self:GetControlObject("runwithos")
	local AutoStartDownloading = self:GetControlObject("startasadd")
	local DefaultTorrentPath = self:GetControlObject("torrentfolder.input")
	local MaxParallelTask = self:GetControlObject("maxdownloadtask.input")
	local MaxDownloadSpeed = self:GetControlObject("maxdownloadspeed.input")
	local MaxUploadSpeed = self:GetControlObject("maxuploadspeed.input")
	local savefolder = self:GetControlObject("savefolder.input")
	local DownloadNotificationSound = self:GetControlObject("beepasfinished")
	
	--newConfig.runwithos = runwithos:GetCheck()
	if AutoStartDownloading:GetCheck() then
		newConfig.AutoStartDownloading = 1
	else
		newConfig.AutoStartDownloading = 0
	end
	newConfig.DefaultTorrentPath = DefaultTorrentPath:GetText()
	newConfig.MaxParallelTask = MaxParallelTask:GetText()
	newConfig.MaxDownloadSpeed = MaxDownloadSpeed:GetText()
	newConfig.MaxUploadSpeed = MaxUploadSpeed:GetText()
	newConfig.DefaultDownloadPath = savefolder:GetText()
	if DownloadNotificationSound:GetCheck() then
		newConfig.DownloadNotificationSound = 1
	else
		newConfig.DownloadNotificationSound = 0
	end
	
	return newConfig
end
--应用设置，更新mainframe UserData
function OnAppplyClick(self)
	local owner = self:GetOwnerControl()
	local hostwndManager = XLGetObject("Xunlei.UIEngine.HostWndManager")
	local config = owner:GetNewConfig()
	local coolClientProxy = XLGetObject('CoolDown.CoolClient.Proxy')
	for k,v in pairs(config) do
		coolClientProxy:SetConfig(k,v)
	end
	owner:ConfigChangeAni("设置已保存")
end

function OnCloseClick(self)
	local owner = self:GetOwnerControl()
	local newConfig = owner:GetNewConfig()
	local coolClientProxy = XLGetObject('CoolDown.CoolClient.Proxy')
	local hostwndManager = XLGetObject("Xunlei.UIEngine.HostWndManager")
	local hostwnd = hostwndManager:GetHostWnd("MainFrame")
	local userData = hostwnd:GetUserData()
	local treeManager = XLGetObject("Xunlei.UIEngine.TreeManager")                   
	local tree = treeManager:GetUIObjectTree("MainObjectTree")
	local tabheader = tree:GetUIObject("tabHeader")
	
	for k,v in pairs(newConfig) do
		if v ~= coolClientProxy:GetConfig(k) then
			local templateManager = XLGetObject("Xunlei.UIEngine.TemplateManager")
			local hostWndManager = XLGetObject("Xunlei.UIEngine.HostWndManager")
			local mainWnd = hostWndManager:GetHostWnd("MainFrame")
			local modalHostWndTemplate = templateManager:GetTemplate("Thunder.MessageBox","HostWndTemplate")
			local modalHostWnd = modalHostWndTemplate:CreateInstance("Thunder.MessageBox.Instance")
			local objectTreeTemplate = templateManager:GetTemplate("Thunder.MessageBox","ObjectTreeTemplate")
			local uiObjectTree = objectTreeTemplate:CreateInstance("Thunder.MessageBox.Instance")
			modalHostWnd:BindUIObjectTree(uiObjectTree)

			local userData = {Title = "设置中心", Icon="bitmap.confirmmodal.warning", Content="提示:是否保存设置变更？", 
			Object = owner, EventName = "OnCloseAndSaveConfirm"}
			modalHostWnd:SetUserData(userData)
			modalHostWnd:DoModal(mainWnd:GetWndHandle())

			local objtreeManager = XLGetObject("Xunlei.UIEngine.TreeManager")	
			objtreeManager:DestroyTree("Thunder.MessageBox.Instance")
			hostWndManager:RemoveHostWnd("Thunder.MessageBox.Instance")
			break
		end
	end
	userData.ConfigPage = false
	AsynCall(function() tabheader:RemoveTabItem('ConfigPage',"MydownloadPage") end)
end

function OnCloseAndSaveConfirm(self)
	local coolClientProxy = XLGetObject('CoolDown.CoolClient.Proxy')
	local hostwndManager = XLGetObject("Xunlei.UIEngine.HostWndManager")
	local hostwnd = hostwndManager:GetHostWnd("MainFrame")
	local userData = hostwnd:GetUserData()
	local config = self:GetNewConfig()
	for k,v in pairs(config) do
		coolClientProxy:SetConfig(k,v)
	end
end