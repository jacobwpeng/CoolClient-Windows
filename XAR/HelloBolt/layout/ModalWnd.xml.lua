--��������ʱ����
--Ŀǰ����������ʾ
function OnCreate(self)
	self:Center()
end

--ȷ�����񴴽�
local function AlphaAni(self)	
	local tmp = self:GetAlpha()

	if tmp > 0 then
		self:SetAlpha(tmp-15,true)
	end
	
end
function OnNewTaskDlgOKClick(self)
	local owner = self:GetOwner()
	local hostwnd = owner:GetBindHostWnd()
	
	local layout = owner:GetUIObject("newtaskdlg.root")
	
	local counter1 = SetTimer(function() AlphaAni(layout) end, 29)
	local counter2 = SetOnceTimer(function() hostwnd:EndDialog(0) end, 520)
	local counter3 = SetOnceTimer(function() KillTimer(counter1) end, 520)
end

function OnNewTaskDlgCancelClick(self)
	local owner = self:GetOwner()
	local hostwnd = owner:GetBindHostWnd()
	hostwnd:EndDialog(0)
end

--����������ťЧ���ĺ���
function OnNewTaskDlgButtonEnter(self)
	self:SetTextureID("texture.dlg.button_hover")
end
function OnNewTaskDlgButtonLeave(self)
	self:SetTextureID("texture.dlg.button_normal")
end

--�༭��ķ���Ч��
function OnNewTaskDlgEditFocusChange(self, isFocus)
	local parent = self:GetParent()
	if isFocus then
		parent:SetTextureID("texture.edit.light.bkg")
	end
end