function OnFocusChange(self, isFocus)
	local parent = self:GetParent()
	if isFocus then
		parent:SetTextureID("texture.configpage.edit.active")
	else
		parent:SetTextureID("")
	end
end

function OnEditAreaInit(self)
	local left,top,right,bottom = self:GetObjPos()
	--XLMessageBox(left.." "..top.." "..right.." "..bottom)
	local attr = self:GetAttribute()
	local edit = self:GetControlObject("input.edit")
	edit:SetIsNumber(attr.Number)
end

function SetText(self, text)
	local edit = self:GetControlObject("input.edit")
	edit:SetText(text)
end

function GetText(self)
	local edit = self:GetControlObject("input.edit")
	return edit:GetText()
end