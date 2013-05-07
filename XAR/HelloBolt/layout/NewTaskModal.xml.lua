function OnCreate(self)
	self:Center()
end
function OnCancelClick(self)
	local owner = self:GetOwner()
	local hostwnd = owner:GetBindHostWnd()
	hostwnd:EndDialog(0)
end