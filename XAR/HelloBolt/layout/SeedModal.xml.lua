function OnCreate(self)
	self:Center()
end

function OnRadioButtonContainerInit(self)
	self:AddRadioButton("btn1","电影",0,0,60,24)
	self:AddRadioButton("btn2","音乐",80,0,60,24)
	self:AddRadioButton("btn3","游戏",160,0,60,24)
	self:AddRadioButton("btn4","图书",240,0,60,24)
end
function OnOKClick(self)
	local owner = self:GetOwner()
	local hostwnd = owner:GetBindHostWnd()
	local userData = hostwnd:GetUserData()
	local path = owner:GetUIObject("path")
	local tracker = owner:GetUIObject("tracker")
	local btnGroup = owner:GetUIObject("RadioGroup")
	local des = owner:GetUIObject("description")
	local data = {test = "Got it"}
	userData.Object:FireExtEvent(userData.EventName, data)
	hostwnd:EndDialog(0)
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
	
end