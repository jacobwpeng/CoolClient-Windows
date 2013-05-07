

function OnInitDisableBtn(self)
	self:SetEnable(false)
end

function OnClickImageBtn(self)
	XLMessageBox("Click close image btn!")
end

function OnUserRadioButtonClick(self)
	self:SetSelect(not self:GetSelect())
end

function OnRadioButtonContainerInit(self)
	self:AddRadioButton("btn1","分组单选1",0,0,100,24)
	self:AddRadioButton("btn2","分组单选2",0,28,100,24)
	self:AddRadioButton("btn3","分组单选3",0,56,100,24)
end