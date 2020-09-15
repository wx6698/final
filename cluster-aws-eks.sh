#!/bin/bash
#
# Tests for cluster-aws-eks playbook.
set -e


pip3 install awscli yamllint cfn-lint ansible-lint ansible openshift


# Export AWS vars.
export AWS_DEFAULT_REGION=us-east-1

# Validate Cloudformation templates.
echo "Validating CloudFormation templates..."
for template in cloudformation/*.yml; do
  yamllint --strict $template
  cfn-lint --include-checks I --template $template
  # validate-template requires valid AWS Credentials :(
  # aws cloudformation validate-template --template-body file://$template --region us-east-1
done

# Check ansible playbooks for correctness.
playbooks="main.yml deploy.yml delete.yml"
echo "Checking playbook syntax..."
ansible-playbook -i inventory $playbooks --syntax-check
echo "Linting playbooks with ansible-lint..."
ansible-lint $playbooks