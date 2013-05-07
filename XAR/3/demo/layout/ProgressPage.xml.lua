

function OnSliderChange(self)
	local prg1 = self:GetOwnerControl():GetControlObject("progress1")
	local prg2 = self:GetOwnerControl():GetControlObject("progress2")
	prg1:SetProgress(self:GetPos())
	prg2:SetProgress(self:GetPos())
end