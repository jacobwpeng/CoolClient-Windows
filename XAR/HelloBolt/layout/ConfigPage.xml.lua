function OnPageInit(self)
	self:SetObjPos(0,0,"father.width","father.height")
end

--弹出确认对话框，是否还原默认设置
function OnResetButtonClick(self)
	local templateManager = XLGetObject("Xunlei.UIEngine.TemplateManager")
	local hostWndManager = XLGetObject("Xunlei.UIEngine.HostWndManager")
	local mainWnd = hostWndManager:GetHostWnd("MainFrame")
	local modalHostWndTemplate = templateManager:GetTemplate("Thunder.ConfigResetModal","HostWndTemplate")
	local modalHostWnd = modalHostWndTemplate:CreateInstance("Thunder.ConfigResetModal.Instance")
	local objectTreeTemplate = templateManager:GetTemplate("Thunder.ConfigResetModal","ObjectTreeTemplate")
	local uiObjectTree = objectTreeTemplate:CreateInstance("Thunder.ConfigResetModal.Instance")
	modalHostWnd:BindUIObjectTree(uiObjectTree)
	
	
	modalHostWnd:DoModal(mainWnd:GetWndHandle())
	
	local objtreeManager = XLGetObject("Xunlei.UIEngine.TreeManager")	
	objtreeManager:DestroyTree("Thunder.ConfigResetModal.Instance")
	hostWndManager:RemoveHostWnd("Thunder.ConfigResetModal.Instance")
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
function ConfigReset(self)
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
	
	runwithos:SetCheck(true)
	startasadd:SetCheck(true)
	torrentfolder:SetText("C:\\cd\\")
	maxdownloadtask:SetText(10)
	maxdownloadspeed:SetText(0)
	maxuploadspeed:SetText(1024)
	savefolder:SetText("C:\\downloads\\")
	beepasfinished:SetCheck(true)
	alertasfinished:SetCheck(true)
	
	self:ConfigChangeAni("已恢复默认设置")
	--XLMessageBox("succ")
end

function GetNewConfig(self)
	local newConfig = {}
	local runwithos = self:GetControlObject("runwithos")
	local startasadd = self:GetControlObject("startasadd")
	local torrentfolder = self:GetControlObject("torrentfolder.input")
	local maxdownloadtask = self:GetControlObject("maxdownloadtask.input")
	local maxdownloadspeed = self:GetControlObject("maxdownloadspeed.input")
	local maxuploadspeed = self:GetControlObject("maxuploadspeed.input")
	local savefolder = self:GetControlObject("savefolder.input")
	local beepasfinished = self:GetControlObject("beepasfinished")
	local alertasfinished = self:GetControlObject("alertasfinished") 
	
	newConfig.runwithos = runwithos:GetCheck()
	newConfig.startasadd = startasadd:GetCheck()
	newConfig.torrentfolder = torrentfolder:GetText()
	newConfig.maxdownloadtask = maxdownloadtask:GetText()
	newConfig.maxdownloadspeed = maxdownloadspeed:GetText()
	newConfig.maxuploadspeed = maxuploadspeed:GetText()
	newConfig.savefolder = savefolder:GetText()
	newConfig.beepasfinished = beepasfinished:GetCheck()
	newConfig.alertasfinished = alertasfinished:GetCheck()
	
	return newConfig
end
--应用设置，更新mainframe UserData
function OnAppplyClick(self)
	local owner = self:GetOwnerControl()
	local hostwndManager = XLGetObject("Xunlei.UIEngine.HostWndManager")
	local hostwnd = hostwndManager:GetHostWnd("MainFrame")
	local userData = hostwnd:GetUserData()
	userData.Config = owner:GetNewConfig()
	
	owner:ConfigChangeAni("设置已保存")
end

function OnCloseClick(self)
	--先检查设置是否发生变动
	--[[
		local isChanged = check()
		if isChanged then
			DoModal()
		else
			Close()
		end
	]]
end