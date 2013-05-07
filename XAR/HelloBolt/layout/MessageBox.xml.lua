function OnCreate(self)
	self:Center()
	local userData = self:GetUserData()
	local tree = self:GetBindUIObjectTree()
	local title = tree:GetUIObject("title")
	local icon = tree:GetUIObject("icon")
	local content = tree:GetUIObject("content")
	
	icon:SetResID(userData.Icon)
	title:SetText(userData.Title)
	content:SetText(userData.Content)
end

function OnOKClick(self)
	local owner = self:GetOwner()
	local hostwnd = owner:GetBindHostWnd()
	local userData = hostwnd:GetUserData()

	userData.Object:FireExtEvent(userData.EventName)
	hostwnd:EndDialog(0)
end
function OnCancelClick(self)
	local owner = self:GetOwner()
	local hostwnd = owner:GetBindHostWnd()

	hostwnd:EndDialog(0)
end

