function OnLButtonDown(self)
	local bkg = self:GetControlObject("button.bkg")
	local attr = self:GetAttribute()
	
	bkg:SetResID(attr.DownBkg)
	self:FireExtEvent("OnClick")
end

function OnLButtonUp(self)
	local bkg = self:GetControlObject("button.bkg")
	local attr = self:GetAttribute()
	
	bkg:SetResID(attr.NormalBkg)

end

function OnMouseEnter(self)
	local bkg = self:GetControlObject("button.bkg")
	local attr = self:GetAttribute()
	
	bkg:SetResID(attr.HoverBkg)
end

function OnInitControl(self)
	local bkg = self:GetControlObject("button.bkg")
	local attr = self:GetAttribute()
	
	bkg:SetResID(attr.NormalBkg)
end

function OnMouseLeave(self)
	local bkg = self:GetControlObject("button.bkg")
	local attr = self:GetAttribute()
	
	bkg:SetResID(attr.NormalBkg)
end