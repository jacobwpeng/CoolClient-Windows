function OnCreate(self)
	self:Center()
end

function OnRadioButtonContainerInit(self)
	self:AddRadioButton("movie","电影",0,0,60,24)
	self:AddRadioButton("music","音乐",80,0,60,24)
	self:AddRadioButton("game","游戏",160,0,60,24)
	self:AddRadioButton("book","图书",240,0,60,24)
end
function OnOKClick(self)
	local owner = self:GetOwner()
	local bkg = owner:GetUIObject("bkg")
	local hostwnd = owner:GetBindHostWnd()
	
	if self:GetText() == "确定" then
		hostwnd:EndDialog(0)
		return 
	end
	
	local userData = hostwnd:GetUserData()
	local path = owner:GetUIObject("path")
	local tracker = owner:GetUIObject("tracker")
	local btnGroup = owner:GetUIObject("RadioGroup")
	local des = owner:GetUIObject("description")
	local data = {}
	local xarManager = XLGetObject("Xunlei.UIEngine.XARManager")
	local xarFactory = xarManager:GetXARFactory()
	local seedMakeProgress = xarFactory:CreateUIObject("seedmakeprogress","Thunder.Progress")
	local seedtype
	if btnGroup:GetSelectedButtonID() == "movie" then
		seedtype = 1
	elseif btnGroup:GetSelectedButtonID() == "music" then
		seedtype = 2 
	elseif btnGroup:GetSelectedButtonID() == "music" then
		seedtype = 4
	else 
		seedtype = 8
	end
	seedMakeProgress:SetObjPos2(30,"father.height - 65", "father.width - 60", 13)
	self:SetEnable(false)
	bkg:AddChild(seedMakeProgress)
	local coolClientProxy = XLGetObject('CoolDown.CoolClient.Proxy')
	coolClientProxy:MakeTorrentAndPublish(path:GetText(), seedtype, tracker:GetText(), des:GetText(), 
		function(cur, total)
			seedMakeProgress:SetProgress((cur/total)*100)
			if cur == total then
				self:SetEnable(true)
				self:SetText("确定")
			end
		end)
end

function OnCancelClick(self)
	local owner = self:GetOwner()
	local okBtn = owner:GetUIObject("ok")
	local hostWnd = owner:GetBindHostWnd()
	if okBtn:GetEnable() == false then
		--停止做种
		okBtn:SetEnable(true)
	else
		hostWnd:EndDialog(0)
	end
end
function OnAddFile(self)
	local owner = self:GetOwner()
	local path = owner:GetUIObject("path")
	local coolClientProxy = XLGetObject('CoolDown.CoolClient.Proxy')
	coolClientProxy:ChoosePath(1, path:GetText(), function(str) path:SetText(str) end )
end

function OnAddDirectory(self)
	local owner = self:GetOwner()
	local path = owner:GetUIObject("path")
	local coolClientProxy = XLGetObject('CoolDown.CoolClient.Proxy')
	coolClientProxy:ChoosePath(2, path:GetText(), function(str) path:SetText(str) end )
end