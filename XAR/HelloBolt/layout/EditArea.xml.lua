function OnFocusChange(self, isFocus)
	local parent = self:GetParent()
	if isFocus then
		parent:SetTextureID("texture.configpage.edit.active")
	else
		parent:SetTextureID("texture.configpage.edit.normal")
	end
end

function OnEditAreaInit(self)
	local left,top,right,bottom = self:GetObjPos()
	--XLMessageBox(left.." "..top.." "..right.." "..bottom)
	local attr = self:GetAttribute()
	local edit = self:GetControlObject("input.edit")
	edit:SetIsNumber(attr.Number)
	if attr.HasBtn then
		edit:SetObjPos2(3,5,"father.width - 30","father.height - 10")
	else
		edit:SetObjPos2(3,5,"father.width - 6","father.height - 10")
	end
	if attr.Multiline then
		edit:SetMultiline(true)
	else
		edit:SetMultiline(false)
	end
	if attr.Readonly then
		edit:SetReadOnly(true)
	else
		edit:SetReadOnly(false)
	end
	edit:SetText(attr.Text)
end

function SetText(self, text)
	local edit = self:GetControlObject("input.edit")
	edit:SetText(text)
end

function GetText(self)
	local edit = self:GetControlObject("input.edit")
	return edit:GetText()
end

function OnKeyDown(self)
	local isFocus = self:GetFocus()
	local ctrl = self:GetOwnerControl()
	if isFocus then
		ctrl:FireExtEvent("OnTextChanged")
	end
end